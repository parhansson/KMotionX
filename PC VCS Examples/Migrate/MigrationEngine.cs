using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Security.Cryptography;
using System.Text;

namespace Migrate
{
    public sealed class MigrationFile
    {
        public string RelativePath { get; internal set; }
        public string SourcePath { get; internal set; }
        public string DestinationPath { get; internal set; }
        public string Reason { get; internal set; }
        public bool Overwrite { get; internal set; }
        internal string SourceHash, DestinationHash;
    }

    public sealed class MigrationPlan
    {
        public string SourceRoot { get; internal set; }
        public string DestinationRoot { get; internal set; }
        public List<MigrationFile> DataFiles { get; private set; }
        public List<MigrationFile> OptionalFiles { get; private set; }
        public List<string> Warnings { get; private set; }
        internal readonly Dictionary<string, TextDocument> Documents = new Dictionary<string, TextDocument>(StringComparer.OrdinalIgnoreCase);
        internal readonly Dictionary<string, MigrationFile> ObservedFiles = new Dictionary<string, MigrationFile>(StringComparer.OrdinalIgnoreCase);
        internal readonly List<MigrationFile> AllDataFiles = new List<MigrationFile>();
        internal readonly List<MigrationFile> AllOptionalFiles = new List<MigrationFile>();
        internal readonly Dictionary<string, string> ReferenceDestinations = new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);
        internal readonly List<string> DataDirectories = new List<string>();
        public MigrationPlan()
        {
            DataFiles = new List<MigrationFile>();
            OptionalFiles = new List<MigrationFile>();
            Warnings = new List<string>();
        }
    }

    public sealed class MigrationResult
    {
        public string BackupDirectory { get; internal set; }
        public int FilesCopied { get; internal set; }
        public List<string> Warnings { get; internal set; }
    }

    internal sealed class TextDocument
    {
        internal string Text;
        internal Encoding Encoding;
        internal byte[] Preamble;
        internal List<PathReference> References;
        internal readonly HashSet<int> ScreenConflicts = new HashSet<int>();
        internal bool ConvertedToUtf8;

        internal static TextDocument Read(string path)
        {
            byte[] bytes = File.ReadAllBytes(path);
            Encoding encoding = new UTF8Encoding(false, true);
            int skip = 0;
            if (bytes.Length >= 4 && bytes[0] == 0xff && bytes[1] == 0xfe && bytes[2] == 0 && bytes[3] == 0)
            { encoding = new UTF32Encoding(false, true); skip = 4; }
            else if (bytes.Length >= 4 && bytes[0] == 0 && bytes[1] == 0 && bytes[2] == 0xfe && bytes[3] == 0xff)
            { encoding = new UTF32Encoding(true, true); skip = 4; }
            else if (bytes.Length >= 3 && bytes[0] == 0xef && bytes[1] == 0xbb && bytes[2] == 0xbf) skip = 3;
            else if (bytes.Length >= 2 && bytes[0] == 0xff && bytes[1] == 0xfe) { encoding = Encoding.Unicode; skip = 2; }
            else if (bytes.Length >= 2 && bytes[0] == 0xfe && bytes[1] == 0xff) { encoding = Encoding.BigEndianUnicode; skip = 2; }
            string text;
            try { text = encoding.GetString(bytes, skip, bytes.Length - skip); }
            catch (DecoderFallbackException) { encoding = Encoding.Default; text = encoding.GetString(bytes); skip = 0; }
            return new TextDocument { Text = text, Encoding = encoding, Preamble = bytes.Take(skip).ToArray() };
        }

        internal byte[] GetBytes(string text)
        {
            Encoding strict = (Encoding)Encoding.Clone();
            strict.EncoderFallback = EncoderFallback.ExceptionFallback;
            try { return Preamble.Concat(strict.GetBytes(text)).ToArray(); }
            catch (EncoderFallbackException)
            {
                // Old ANSI files cannot represent every installation folder name.
                // KMotion's UTF-8 readers support a BOM; never substitute '?' in paths.
                var utf8 = new UTF8Encoding(true, true);
                ConvertedToUtf8 = true;
                return utf8.GetPreamble().Concat(utf8.GetBytes(text)).ToArray();
            }
        }
    }

    public static class MigrationEngine
    {
        public static MigrationPlan BuildPlan(string sourceRoot, string destinationRoot)
        {
            var plan = new MigrationPlan { SourceRoot = InstallationPaths.NormalizeRoot(sourceRoot), DestinationRoot = InstallationPaths.NormalizeRoot(destinationRoot) };
            ValidateRoots(plan);
            var files = plan.ObservedFiles;
            var queue = new Queue<MigrationFile>();
            GatherData(plan, Path.Combine(plan.SourceRoot, "KMotion", "Data"), files, queue);
            foreach (string config in new[] { "GCodeConfigCNC.txt", "Threads.txt" })
                if (!File.Exists(Path.Combine(plan.SourceRoot, "KMotion", "Data", config)))
                    plan.Warnings.Add(config + " was not found in the previous Data folder.");

            while (queue.Count > 0)
            {
                MigrationFile file = queue.Dequeue();
                string name = Path.GetFileName(file.SourcePath);
                string extension = Path.GetExtension(name).ToLowerInvariant();
                if (!name.Equals("GCodeConfigCNC.txt", StringComparison.OrdinalIgnoreCase) &&
                    !name.Equals("GCodeConfigCNC.txt.bak", StringComparison.OrdinalIgnoreCase) &&
                    !name.Equals("Threads.txt", StringComparison.OrdinalIgnoreCase) &&
                    extension != ".scr" && extension != ".c" && extension != ".h") continue;
                TextDocument document = TextDocument.Read(file.SourcePath);
                document.References = ReferenceScanner.Scan(file.SourcePath, document.Text, plan.SourceRoot, plan.Warnings);
                foreach (var conflict in ReferenceScanner.FindScreenConflicts(file.DestinationPath, document.Text,
                    plan.SourceRoot, plan.DestinationRoot, document.References))
                {
                    document.ScreenConflicts.Add(conflict.Key);
                    plan.ReferenceDestinations[conflict.Value] = FileSafety.Fingerprint(conflict.Value);
                }
                plan.Documents[file.SourcePath] = document;
                foreach (PathReference reference in document.References)
                {
                    try
                    {
                        FileSafety.RequireWithin(reference.SourcePath, plan.SourceRoot);
                        string relative = reference.SourcePath.Substring(plan.SourceRoot.Length + 1);
                        string destination = Path.Combine(plan.DestinationRoot, relative);
                        FileSafety.RequireWithin(destination, plan.DestinationRoot);
                        if (files.ContainsKey(reference.SourcePath)) continue;
                        if (!File.Exists(reference.SourcePath))
                        {
                            plan.ReferenceDestinations[destination] = FileSafety.Fingerprint(destination);
                            // Visibility belongs to this reference. Other screens
                            // reuse the same control ID for unrelated labels/actions.
                            if (!reference.IsHiddenImage && !reference.IsHiddenProgram && !File.Exists(destination))
                                plan.Warnings.Add("Referenced file is missing or is a folder: " + reference.SourcePath + " (" + reference.Reason + ")");
                            continue;
                        }
                        MigrationFile extra = CreateFile(plan, reference.SourcePath, reference.Reason);
                        files.Add(extra.SourcePath, extra);
                        plan.OptionalFiles.Add(extra);
                        queue.Enqueue(extra);
                    }
                    catch (IOException ex) { plan.Warnings.Add(ex.Message); }
                    catch (UnauthorizedAccessException ex) { plan.Warnings.Add(ex.Message); }
                }
            }
            plan.AllDataFiles.AddRange(plan.DataFiles);
            plan.AllOptionalFiles.AddRange(plan.OptionalFiles);
            // Scan dependencies even when the referencing file is identical. A
            // matching screen/program may still need a missing asset or include.
            RefreshDataFiles(plan, Enumerable.Empty<MigrationFile>());
            return plan;
        }

        public static void RefreshDataFiles(MigrationPlan plan, IEnumerable<MigrationFile> selectedFiles)
        {
            if (plan == null) throw new ArgumentNullException("plan");
            var selected = ValidateSelection(plan, selectedFiles);
            var available = AvailableFiles(plan, selected);
            plan.DataFiles.Clear();
            plan.DataFiles.AddRange(plan.AllDataFiles.Where(file => IsDifferent(plan, file, available)));
            plan.DataFiles.Sort((a, b) => StringComparer.OrdinalIgnoreCase.Compare(a.RelativePath, b.RelativePath));
            plan.OptionalFiles.Clear();
            plan.OptionalFiles.AddRange(plan.AllOptionalFiles.Where(file => IsDifferent(plan, file, available)));
            plan.OptionalFiles.Sort((a, b) => StringComparer.OrdinalIgnoreCase.Compare(a.RelativePath, b.RelativePath));
        }

        private static HashSet<MigrationFile> ValidateSelection(MigrationPlan plan, IEnumerable<MigrationFile> selectedFiles)
        {
            var selected = new HashSet<MigrationFile>(selectedFiles ?? Enumerable.Empty<MigrationFile>());
            if (selected.Any(item => !plan.AllOptionalFiles.Contains(item))) throw new IOException("Invalid selection. Preview again.");
            return selected;
        }

        private static HashSet<string> AvailableFiles(MigrationPlan plan, HashSet<MigrationFile> selected)
        {
            return new HashSet<string>(plan.AllDataFiles.Concat(selected).Select(file => file.SourcePath), StringComparer.OrdinalIgnoreCase);
        }

        private static bool IsDifferent(MigrationPlan plan, MigrationFile file, HashSet<string> available)
        {
            if (file.DestinationHash == null) return true;
            TextDocument document;
            if (!plan.Documents.TryGetValue(file.SourcePath, out document)) return file.SourceHash != file.DestinationHash;
            byte[] bytes = document.GetBytes(Rewrite(plan, file, document, available, new List<string>()));
            using (var hash = SHA256.Create())
                return Convert.ToBase64String(hash.ComputeHash(bytes)) != file.DestinationHash;
        }

        private static void ValidateRoots(MigrationPlan plan)
        {
            if (String.Equals(plan.SourceRoot, plan.DestinationRoot, StringComparison.OrdinalIgnoreCase) ||
                FileSafety.IsWithin(plan.SourceRoot, plan.DestinationRoot) || FileSafety.IsWithin(plan.DestinationRoot, plan.SourceRoot))
                throw new IOException("Previous and new installations must be separate, non-overlapping folders.");
            FileSafety.NoLinks(Path.Combine(plan.SourceRoot, "KMotion", "Data"));
            FileSafety.NoLinks(Path.Combine(plan.DestinationRoot, "KMotion", "Data"));
        }

        private static MigrationFile CreateFile(MigrationPlan plan, string source, string reason)
        {
            source = FileSafety.Canonical(source);
            FileSafety.RequireWithin(source, plan.SourceRoot);
            string relative = source.Substring(plan.SourceRoot.Length + 1);
            string destination = Path.Combine(plan.DestinationRoot, relative);
            FileSafety.RequireWithin(destination, plan.DestinationRoot);
            string sourceHash = FileSafety.Fingerprint(source);
            if (sourceHash == null) throw new IOException("Source file no longer exists: " + source);
            string destinationHash = FileSafety.Fingerprint(destination);
            return new MigrationFile { SourcePath = source, DestinationPath = destination, RelativePath = relative,
                Reason = reason, SourceHash = sourceHash, DestinationHash = destinationHash, Overwrite = destinationHash != null };
        }

        private static void GatherData(MigrationPlan plan, string directory, Dictionary<string, MigrationFile> files, Queue<MigrationFile> queue)
        {
            FileSafety.RequireWithin(directory, plan.SourceRoot);
            plan.DataDirectories.Add(directory.Substring(plan.SourceRoot.Length + 1));
            foreach (string file in Directory.GetFiles(directory))
            {
                if ((File.GetAttributes(file) & FileAttributes.ReparsePoint) != 0)
                { plan.Warnings.Add("Skipped linked Data file: " + file); continue; }
                MigrationFile item = CreateFile(plan, file, "Previous Data settings");
                plan.DataFiles.Add(item);
                files.Add(item.SourcePath, item);
                queue.Enqueue(item);
            }
            foreach (string child in Directory.GetDirectories(directory))
            {
                if ((File.GetAttributes(child) & FileAttributes.ReparsePoint) != 0)
                { plan.Warnings.Add("Skipped linked Data folder: " + child); continue; }
                GatherData(plan, child, files, queue);
            }
        }

        public static MigrationResult Execute(MigrationPlan plan, IEnumerable<MigrationFile> selectedFiles, Action<string> progress)
        {
            if (plan == null) throw new ArgumentNullException("plan");
            ValidateRoots(plan);
            var selected = ValidateSelection(plan, selectedFiles);
            // Include skipped files in validation: a file that matched at preview
            // must not become a silent, unreviewed difference before the merge.
            foreach (MigrationFile file in plan.ObservedFiles.Values) VerifyFile(plan, file);
            foreach (var destination in plan.ReferenceDestinations)
                if (FileSafety.Fingerprint(destination.Key) != destination.Value)
                    throw new IOException("A referenced destination changed since preview. Preview again: " + destination.Key);
            RefreshDataFiles(plan, selected);
            var available = AvailableFiles(plan, selected);
            var files = plan.DataFiles.Concat(plan.OptionalFiles.Where(selected.Contains))
                .Where(file => IsDifferent(plan, file, available)).ToList();
            var warnings = new List<string>(plan.Warnings);
            if (files.Count == 0)
                return new MigrationResult { BackupDirectory = null, FilesCopied = 0, Warnings = warnings };
            CheckRunningApplications(plan);

            string backup = Path.Combine(plan.DestinationRoot, "MigrateBackups", DateTime.Now.ToString("yyyyMMdd-HHmmss") + "-" + Guid.NewGuid().ToString("N").Substring(0, 8));
            FileSafety.RequireWithin(backup, plan.DestinationRoot);
            Directory.CreateDirectory(backup);
            string staged = Path.Combine(backup, "Prepared");
            var touched = new List<MigrationFile>();
            var log = new List<string> { "Migrate started " + DateTime.Now.ToString("O"), "Previous: " + plan.SourceRoot, "New: " + plan.DestinationRoot };
            string logPath = Path.Combine(backup, "migration.log");
            try
            {
                // Prepare all bytes and backups before modifying the destination.
                foreach (MigrationFile file in files)
                {
                    if (progress != null) progress("Preparing " + file.RelativePath);
                    VerifyFile(plan, file);
                    string stagedPath = Path.Combine(staged, file.RelativePath);
                    Directory.CreateDirectory(Path.GetDirectoryName(stagedPath));
                    TextDocument document;
                    if (plan.Documents.TryGetValue(file.SourcePath, out document))
                    {
                        File.WriteAllBytes(stagedPath, document.GetBytes(Rewrite(plan, file, document, available, warnings)));
                        if (document.ConvertedToUtf8) warnings.Add("Saved as UTF-8 to preserve the new folder name: " + file.RelativePath);
                    }
                    else
                        File.Copy(file.SourcePath, stagedPath, false);
                    // Parsed documents are from preview; all other bytes are verified after staging.
                    if (!plan.Documents.ContainsKey(file.SourcePath) && FileSafety.Fingerprint(stagedPath) != file.SourceHash)
                        throw new IOException("Source changed during copying. Preview again: " + file.SourcePath);
                    if (file.Overwrite)
                    {
                        string original = Path.Combine(backup, "Original", file.RelativePath);
                        Directory.CreateDirectory(Path.GetDirectoryName(original));
                        File.Copy(file.DestinationPath, original, false);
                        if (FileSafety.Fingerprint(original) != file.DestinationHash)
                            throw new IOException("Destination changed during backup. Preview again: " + file.DestinationPath);
                    }
                }
                File.WriteAllLines(logPath, log, Encoding.UTF8);
                CheckRunningApplications(plan);
                foreach (string relative in plan.DataDirectories)
                {
                    string directory = Path.Combine(plan.DestinationRoot, relative);
                    FileSafety.RequireWithin(directory, plan.DestinationRoot);
                    Directory.CreateDirectory(directory);
                }
                foreach (MigrationFile file in files)
                {
                    if (progress != null) progress("Copying " + file.RelativePath);
                    VerifyFile(plan, file);
                    Directory.CreateDirectory(Path.GetDirectoryName(file.DestinationPath));
                    // Record intent before changing bytes so interrupted copies can be recovered.
                    log.Add((file.Overwrite ? "REPLACE " : "CREATE ") + file.RelativePath);
                    File.WriteAllLines(logPath, log, Encoding.UTF8);
                    string preparedFile = Path.Combine(staged, file.RelativePath);
                    // Both paths are on the destination volume. Atomic operations avoid
                    // partial file contents and do not overwrite a newly appeared file.
                    if (file.Overwrite) File.Replace(preparedFile, file.DestinationPath, null);
                    else File.Move(preparedFile, file.DestinationPath);
                    touched.Add(file);
                }
                log.Add("SUCCESS: " + files.Count + " files copied.");
                log.AddRange(warnings.Select(w => "WARNING: " + w));
                File.WriteAllLines(logPath, log, Encoding.UTF8);
                return new MigrationResult { BackupDirectory = backup, FilesCopied = files.Count, Warnings = warnings };
            }
            catch (Exception ex)
            {
                var rollbackFailures = new List<string>();
                foreach (MigrationFile file in touched.AsEnumerable().Reverse())
                {
                    try
                    {
                        FileSafety.RequireWithin(file.DestinationPath, plan.DestinationRoot);
                        if (file.Overwrite) File.Copy(Path.Combine(backup, "Original", file.RelativePath), file.DestinationPath, true);
                        else if (File.Exists(file.DestinationPath)) File.Delete(file.DestinationPath);
                    }
                    catch (Exception restoreError) { rollbackFailures.Add(file.RelativePath + ": " + restoreError.Message); }
                }
                log.Add("FAILED: " + ex.Message);
                log.Add(rollbackFailures.Count == 0 ? "Changes to destination files rolled back." : "ROLLBACK INCOMPLETE:");
                log.AddRange(rollbackFailures);
                try { File.WriteAllLines(logPath, log, Encoding.UTF8); } catch (IOException) { } catch (UnauthorizedAccessException) { }
                throw new IOException("Migration failed: " + ex.Message + Environment.NewLine +
                    (rollbackFailures.Count == 0 ? "Destination file changes were rolled back." : "Some files could not be restored; see migration.log.") +
                    Environment.NewLine + "Backup and log: " + backup, ex);
            }
        }

        private static string Rewrite(MigrationPlan plan, MigrationFile file, TextDocument document, HashSet<string> available, List<string> warnings)
        {
            string text = document.Text;
            foreach (PathReference reference in document.References.OrderByDescending(r => r.Start))
            {
                string destination = Path.Combine(plan.DestinationRoot, reference.SourcePath.Substring(plan.SourceRoot.Length + 1));
                bool useNew = available.Contains(reference.SourcePath);
                try { FileSafety.RequireWithin(destination, plan.DestinationRoot); useNew |= File.Exists(destination); }
                catch (IOException) { useNew = false; }
                string replacement;
                if (useNew)
                {
                    // Relative references resolve in the same installation layout.
                    // Keep their spelling so an otherwise identical file stays identical.
                    if (!reference.WasAbsolute && !document.ScreenConflicts.Contains(reference.Start)) continue;
                    replacement = destination;
                }
                else if (File.Exists(reference.SourcePath))
                {
                    replacement = reference.SourcePath;
                    warnings.Add("Reference still uses the previous installation (file not selected): " + reference.SourcePath);
                }
                else continue;
                // Forward slashes are accepted by the C preprocessor and avoid backslash escapes.
                string extension = Path.GetExtension(file.SourcePath);
                if (extension.Equals(".c", StringComparison.OrdinalIgnoreCase) || extension.Equals(".h", StringComparison.OrdinalIgnoreCase))
                    replacement = replacement.Replace('\\', '/');
                if (reference.QuoteIfNeeded && replacement.Any(Char.IsWhiteSpace)) replacement = "\"" + replacement + "\"";
                text = text.Remove(reference.Start, reference.Length).Insert(reference.Start, replacement);
            }
            return text;
        }

        private static void VerifyFile(MigrationPlan plan, MigrationFile file)
        {
            FileSafety.RequireWithin(file.SourcePath, plan.SourceRoot);
            FileSafety.RequireWithin(file.DestinationPath, plan.DestinationRoot);
            if (FileSafety.Fingerprint(file.SourcePath) != file.SourceHash || FileSafety.Fingerprint(file.DestinationPath) != file.DestinationHash)
                throw new IOException("A file changed since preview. Preview again before merging: " + file.RelativePath);
        }

        private static void CheckRunningApplications(MigrationPlan plan)
        {
            foreach (string name in new[] { "KMotion", "KMotionCNC" })
                foreach (Process process in Process.GetProcessesByName(name))
                    using (process)
                    {
                        try
                        {
                            string executable = process.MainModule.FileName;
                            if (FileSafety.IsWithin(executable, plan.SourceRoot) || FileSafety.IsWithin(executable, plan.DestinationRoot))
                                throw new IOException("Close " + name + " in the previous and new installations before merging settings.");
                        }
                        catch (System.ComponentModel.Win32Exception)
                        { throw new IOException("Close running KMotion/KMotionCNC applications before merging settings."); }
                        catch (InvalidOperationException) { /* Process exited while checking. */ }
                    }
        }
    }
}
