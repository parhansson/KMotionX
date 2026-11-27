using System;
using System.Collections.Generic;
using System.IO;
using System.Text.RegularExpressions;

namespace Migrate
{
    internal sealed class PathReference
    {
        internal int Start;
        internal int Length;
        internal string SourcePath;
        internal string Reason;
        internal bool WasAbsolute;
        internal bool QuoteIfNeeded;
        internal bool IsHiddenImage;
        internal bool IsHiddenProgram;
        internal string ControlId;
    }

    // Only returns paths within the selected installation. Spans refer to the
    // original text, including any leading whitespace, quotes and line endings.
    internal static class ReferenceScanner
    {
        private const string Screens = @"PC VC Examples\KMotionCNC\Screens";
        private const string Bitmaps = @"PC VC Examples\KMotionCNC\res";
        private static readonly Regex Lines = new Regex(@"[^\r\n]+", RegexOptions.Compiled);
        private static readonly Regex ActionKey = new Regex(@"^Interpreter->McodeActions\[(\d+)\]\.(Action|String)$", RegexOptions.Compiled);

        internal static List<PathReference> Scan(string filePath, string text, string sourceRoot, List<string> warnings)
        {
            var scanner = new Scanner(filePath, text, sourceRoot, warnings);
            string extension = Path.GetExtension(filePath).ToLowerInvariant();
            if (String.Equals(Path.GetFileName(filePath), "Threads.txt", StringComparison.OrdinalIgnoreCase))
                scanner.Threads();
            else if (extension == ".scr")
                scanner.Screen();
            else if (extension == ".c" || extension == ".h")
                scanner.Includes();
            else
                scanner.Configuration();
            scanner.Results.Sort(delegate(PathReference a, PathReference b) { return a.Start.CompareTo(b.Start); });
            return scanner.Results;
        }

        private static bool TryScreenControl(string record, out string id, out int show)
        {
            id = null;
            show = -1;
            Match control = Regex.Match(record, @"^\s*ID\s*:\s*([^,\r\n]+)");
            if (!control.Success) return false;
            id = control.Groups[1].Value.Trim();
            Match script = Regex.Match(record, @"(?:^|,)\s*Script\s*:");
            if (script.Success) record = record.Substring(0, script.Index);
            MatchCollection shows = Regex.Matches(record, @"(?:^|,)\s*Show\s*:\s*([^,\r\n]*)");
            return shows.Count > 0 && Int32.TryParse(shows[shows.Count - 1].Groups[1].Value.Trim(), out show);
        }

        // Preserve a relative screen reference only when its destination lookup
        // still finds the corresponding file. New releases may add a competing
        // bitmap/script earlier in the screen resolver's search order.
        internal static Dictionary<int, string> FindScreenConflicts(string destinationFilePath, string text,
            string sourceRoot, string destinationRoot, List<PathReference> originalReferences)
        {
            var conflicts = new Dictionary<int, string>();
            if (text == null || originalReferences == null) return conflicts;
            try
            {
                string destinationFile = FileSafety.Canonical(destinationFilePath);
                if (!String.Equals(Path.GetExtension(destinationFile), ".scr", StringComparison.OrdinalIgnoreCase))
                    return conflicts;
                string source = FileSafety.Canonical(sourceRoot);
                string destination = FileSafety.Canonical(destinationRoot);
                if (String.Equals(source, destination, StringComparison.OrdinalIgnoreCase) ||
                    FileSafety.IsWithin(source, destination) || FileSafety.IsWithin(destination, source))
                    return conflicts;
                FileSafety.NoLinks(source);
                FileSafety.NoLinks(destination);
                FileSafety.RequireWithin(destinationFile, destination);

                List<PathReference> resolved = Scan(destinationFile, text, destination, new List<string>());
                var byStart = new Dictionary<int, PathReference>();
                foreach (PathReference reference in resolved)
                    if (!reference.WasAbsolute) byStart[reference.Start] = reference;
                foreach (PathReference original in originalReferences)
                {
                    if (original == null || original.WasAbsolute || original.Start < 0 || original.Length <= 0 ||
                        original.Start > text.Length - original.Length) continue;
                    PathReference competing;
                    if (!byStart.TryGetValue(original.Start, out competing) || competing.Length != original.Length) continue;
                    try
                    {
                        string originalPath = FileSafety.Canonical(original.SourcePath);
                        FileSafety.RequireWithin(originalPath, source);
                        string expected = FileSafety.Canonical(Path.Combine(destination, originalPath.Substring(source.Length + 1)));
                        FileSafety.RequireWithin(expected, destination);
                        string actual = FileSafety.Canonical(competing.SourcePath);
                        FileSafety.RequireWithin(actual, destination);
                        if (!String.Equals(actual, expected, StringComparison.OrdinalIgnoreCase) && File.Exists(actual))
                            conflicts[original.Start] = actual;
                    }
                    catch (Exception error)
                    {
                        if (!InvalidReferencePath(error)) throw;
                    }
                }
            }
            catch (Exception error)
            {
                if (!InvalidReferencePath(error)) throw;
            }
            return conflicts;
        }

        private static bool InvalidReferencePath(Exception error)
        {
            return error is IOException || error is UnauthorizedAccessException || error is ArgumentException ||
                error is NotSupportedException || error is System.Security.SecurityException;
        }

        private sealed class Scanner
        {
            internal readonly List<PathReference> Results = new List<PathReference>();
            private readonly string file;
            private readonly string text;
            private readonly string root;
            private readonly List<string> warnings;
            private readonly Dictionary<string, string> screenCache = new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);

            internal Scanner(string filePath, string contents, string sourceRoot, List<string> messages)
            {
                file = filePath;
                text = contents;
                root = FileSafety.Canonical(sourceRoot);
                warnings = messages;
            }

            internal void Configuration()
            {
                var actions = new Dictionary<string, int>();
                foreach (Match line in Lines.Matches(text))
                {
                    int equals = line.Value.IndexOf('=');
                    if (equals < 0) continue;
                    Match key = ActionKey.Match(line.Value.Substring(0, equals).Trim());
                    int action;
                    if (key.Success && key.Groups[2].Value == "Action" &&
                        Int32.TryParse(line.Value.Substring(equals + 1).Trim(), out action))
                        actions[key.Groups[1].Value] = action;
                }

                foreach (Match line in Lines.Matches(text))
                {
                    int equals = line.Value.IndexOf('=');
                    if (equals < 0) continue;
                    string key = line.Value.Substring(0, equals).Trim();
                    int start = line.Index + equals + 1;
                    int length = line.Length - equals - 1;
                    string value = line.Value.Substring(equals + 1).Trim();
                    string basis = null;
                    if (key == "m_SetupFile" || key == "m_ToolFile" || key == "m_GeoFile")
                        basis = @"KMotion\Data";
                    else if (key == "m_VarsFile")
                        basis = "GCode Programs";
                    else if (key == "m_ScreenScriptFile")
                        basis = Screens;

                    if (basis != null)
                    {
                        Add(start, length, Path.Combine(root, basis), key, false, false);
                        continue;
                    }

                    Match actionKey = ActionKey.Match(key);
                    if (actionKey.Success && actionKey.Groups[2].Value == "String")
                    {
                        int action;
                        if (!actions.TryGetValue(actionKey.Groups[1].Value, out action)) action = -1;
                        if (action == 4 || action == 5 || action == 6)
                        {
                            basis = value.EndsWith(".ngc", StringComparison.OrdinalIgnoreCase) ? "GCode Programs" : "C Programs";
                            Add(start, length, Path.Combine(root, basis), key, false, false);
                        }
                        else if (action == 10)
                            Add(start, length, Path.Combine(root, Screens), key, true, false);
                        else if (action == 7)
                            Command(start, length, key, false);
                        else
                            AbsoluteTokens(start, length, key);
                    }
                    else
                        AbsoluteTokens(start, length, key);
                }
            }

            internal void Threads()
            {
                foreach (Match line in Lines.Matches(text))
                {
                    if (line.Value.IndexOf('|') >= 0)
                    {
                        Warn("Unrecognized pipe-delimited thread entry was left unchanged: " + line.Value.Trim());
                        continue;
                    }
                    Add(line.Index, line.Length, Path.Combine(root, "C Programs"), "KMotion thread program", false, false);
                }
            }

            internal void Screen()
            {
                // A Script field can itself contain SScript or Action, while
                // ordinary screen fields are separated with commas.
                var fields = new Regex(@"(?:^|,|Script:)(?<key>SScript|ScriptName|BackBitmap|BitmapFile|Action):(?<value>[^,\r\n]*)", RegexOptions.Multiline);
                foreach (Match match in fields.Matches(text))
                {
                    Group value = match.Groups["value"];
                    string key = match.Groups["key"].Value;
                    if (key == "Action")
                    {
                        int offset = 0;
                        int separators = 0;
                        while (offset < value.Length && separators < 6)
                            if (value.Value[offset++] == ';') separators++;
                        if (separators == 6)
                        {
                            int action;
                            int delimiter = value.Value.IndexOf(';');
                            bool knownAction = Int32.TryParse(value.Value.Substring(0, delimiter), out action);
                            if (knownAction && action == 7)
                                Command(value.Index + offset, value.Length - offset, "screen action", true);
                            else
                            {
                                string hiddenControl = null;
                                string program = value.Value.Substring(offset).Trim().Trim('"');
                                if (knownAction && (action == 4 || action == 5 || action == 6) &&
                                    program.EndsWith(".c", StringComparison.OrdinalIgnoreCase))
                                    hiddenControl = HiddenProgramControl(value.Index);
                                Add(value.Index + offset, value.Length - offset, Path.GetDirectoryName(file), "screen action", true, false, false, hiddenControl);
                            }
                        }
                    }
                    else if (key == "BitmapFile")
                    {
                        bool hiddenImage = IsHiddenControlImage(value.Index);
                        int position = 0;
                        foreach (string part in value.Value.Split(';'))
                        {
                            Add(value.Index + position, part.Length, Path.GetDirectoryName(file), "screen bitmap", true, false, hiddenImage);
                            position += part.Length + 1;
                        }
                    }
                    else
                        Add(value.Index, value.Length, Path.GetDirectoryName(file), key, true, false);
                }
            }

            private string HiddenProgramControl(int position)
            {
                int start = position;
                while (start > 0 && text[start - 1] != '\r' && text[start - 1] != '\n') start--;
                int end = position;
                while (end < text.Length && text[end] != '\r' && text[end] != '\n') end++;
                string id;
                int show;
                return TryScreenControl(text.Substring(start, end - start), out id, out show) && show == 0 ? id : null;
            }

            private bool IsHiddenControlImage(int position)
            {
                int start = position;
                while (start > 0 && text[start - 1] != '\r' && text[start - 1] != '\n') start--;
                int end = position;
                while (end < text.Length && text[end] != '\r' && text[end] != '\n') end++;
                string record = text.Substring(start, end - start);
                if (!Regex.IsMatch(record, @"^\s*ID:")) return false;

                // Show belongs to this control, not an embedded script or the
                // preceding control. Screen properties can occur in any order.
                Match script = Regex.Match(record, @"(?:^|,)\s*Script\s*:");
                if (script.Success)
                {
                    if (position - start >= script.Index) return false;
                    record = record.Substring(0, script.Index);
                }
                MatchCollection shows = Regex.Matches(record, @"(?:^|,)\s*Show\s*:\s*([^,\r\n]*)");
                if (shows.Count == 0) return false;
                int show;
                return Int32.TryParse(shows[shows.Count - 1].Groups[1].Value.Trim(), out show) && show == 0;
            }

            internal void Includes()
            {
                // Mask comments without moving offsets. This deliberately does
                // not interpret macros or arbitrary strings as file references.
                string masked = Regex.Replace(text, @"/\*[\s\S]*?\*/|//[^\r\n]*", delegate(Match match)
                {
                    return Regex.Replace(match.Value, @"[^\r\n]", " ");
                });
                var includes = new Regex("^[ \\t]*#[ \\t]*include[ \\t]*\"(?<path>[^\"\\r\\n]+)\"", RegexOptions.Multiline);
                foreach (Match match in includes.Matches(masked))
                {
                    Group path = match.Groups["path"];
                    Add(path.Index, path.Length, Path.GetDirectoryName(file), "quoted C include", false, true);
                }
            }

            private void Command(int start, int length, string reason, bool screenSearch)
            {
                Trim(ref start, ref length, false);
                if (length == 0) return;
                string value = text.Substring(start, length);
                string basis = screenSearch ? Path.GetDirectoryName(file) : Path.Combine(root, Screens);
                if (value[0] == '"')
                {
                    int quote = value.IndexOf('"', 1);
                    if (quote > 1)
                        Add(start + 1, quote - 1, basis, reason + " executable", screenSearch, false);
                }
                else
                {
                    Match executable = Regex.Match(value, @"^.*?\.(?:exe|com|bat|cmd)(?=\s|$)", RegexOptions.IgnoreCase);
                    if (executable.Success)
                        Add(start, executable.Length, basis, reason + " executable", screenSearch, false);
                    else if (value.Length > 0)
                        Warn("Could not identify the executable in " + reason + "; inspect this command after migration.");
                }
                AbsoluteTokens(start, length, reason + " argument");
                foreach (PathReference reference in Results)
                    if (reference.Start >= start && reference.Start + reference.Length <= start + length)
                        reference.QuoteIfNeeded = !(reference.Start > 0 && text[reference.Start - 1] == '"' &&
                            reference.Start + reference.Length < text.Length && text[reference.Start + reference.Length] == '"');
            }

            private void AbsoluteTokens(int start, int length, string reason)
            {
                // Unknown configuration keys may still contain a previous-version
                // path. Quoted values and semicolon-separated lists are supported.
                string value = text.Substring(start, length);
                var tokens = new Regex("[A-Za-z]:[\\\\/][^\"<>|;\\r\\n]*|\\\\\\\\[^\"<>|;\\r\\n]+", RegexOptions.CultureInvariant);
                foreach (Match match in tokens.Matches(value))
                {
                    int tokenLength = match.Length;
                    string candidate = match.Value.TrimEnd();
                    // An executable may be followed by arguments in an unquoted
                    // command; those arguments are not part of the filename.
                    Match executable = Regex.Match(candidate, @"^.*?\.(?:exe|com|bat|cmd)(?=\s|$)", RegexOptions.IgnoreCase);
                    if (executable.Success) tokenLength = executable.Length;
                    Add(start + match.Index, tokenLength, root, reason, false, false);
                }
            }

            private void Add(int start, int length, string basis, string reason, bool screenSearch, bool includeSearch, bool hiddenImage = false, string hiddenProgramControl = null)
            {
                Trim(ref start, ref length, true);
                if (length == 0) return;
                string value = text.Substring(start, length);
                bool absolute = Regex.IsMatch(value, @"^[A-Za-z]:[\\/]|^\\\\");
                if (!absolute && (value.IndexOf(':') >= 0 || value.IndexOf('%') >= 0 || value.IndexOf('|') >= 0))
                {
                    if (!hiddenImage) Warn("Unsupported path in " + reason + " was left unchanged: " + value);
                    return;
                }
                string path;
                try
                {
                    string relative = value.Replace('/', Path.DirectorySeparatorChar).TrimStart(Path.DirectorySeparatorChar);
                    path = FileSafety.Canonical(absolute ? value : Path.Combine(basis, relative));
                    if (!absolute && screenSearch) path = FindScreen(relative, path);
                    if (!absolute && includeSearch && !File.Exists(path))
                    {
                        foreach (string directory in new[] { "C Programs", "DSP_KFLOP", "DSP_KOGNA" })
                        {
                            string alternative = FileSafety.Canonical(Path.Combine(Path.Combine(root, directory), relative));
                            if (FileSafety.IsWithin(alternative, root) && File.Exists(alternative))
                            {
                                path = alternative;
                                break;
                            }
                        }
                        // Nonexistent compiler/platform includes are not copied.
                        if (!File.Exists(path))
                        {
                            Warn("Quoted include was not found in the previous installation: " + value);
                            return;
                        }
                    }
                }
                catch (Exception error)
                {
                    if (!(error is ArgumentException) && !(error is NotSupportedException) && !(error is PathTooLongException)) throw;
                    if (!hiddenImage) Warn("Invalid path in " + reason + " was left unchanged: " + value);
                    return;
                }
                if (!FileSafety.IsWithin(path, root))
                {
                    if (!hiddenImage || File.Exists(path))
                        Warn("Reference outside the selected previous installation was left unchanged: " + value);
                    return;
                }
                foreach (PathReference prior in Results)
                    if (start < prior.Start + prior.Length && prior.Start < start + length) return;
                Results.Add(new PathReference { Start = start, Length = length, SourcePath = path, Reason = reason,
                    WasAbsolute = absolute, IsHiddenImage = hiddenImage,
                    IsHiddenProgram = hiddenProgramControl != null, ControlId = hiddenProgramControl });
            }

            private string FindScreen(string relative, string fallback)
            {
                if (File.Exists(fallback)) return fallback;
                string cached;
                if (screenCache.TryGetValue(relative, out cached)) return cached;
                string screenDirectory = Path.GetDirectoryName(file);
                // Searching KMotion\Data recursively is not the screen resolver's
                // behavior when a screen is referenced from a configuration file.
                if (String.Equals(Path.GetExtension(file), ".scr", StringComparison.OrdinalIgnoreCase))
                {
                    var pending = new Stack<string>();
                    pending.Push(screenDirectory);
                    while (pending.Count > 0)
                    {
                        string directory = pending.Pop();
                        try
                        {
                            FileSafety.NoLinks(directory);
                            if (!FileSafety.IsWithin(directory, root) ||
                                (File.GetAttributes(directory) & FileAttributes.ReparsePoint) != 0) continue;
                            string candidate = FileSafety.Canonical(Path.Combine(directory, relative));
                            if (FileSafety.IsWithin(candidate, root) && File.Exists(candidate))
                            {
                                screenCache[relative] = candidate;
                                return candidate;
                            }
                            string[] children = Directory.GetDirectories(directory);
                            Array.Sort(children, StringComparer.OrdinalIgnoreCase);
                            for (int i = children.Length - 1; i >= 0; i--) pending.Push(children[i]);
                        }
                        catch (UnauthorizedAccessException) { Warn("Could not search screen dependency folder: " + directory); }
                        catch (IOException) { Warn("Could not search screen dependency folder: " + directory); }
                    }
                }
                foreach (string basis in new[] { Screens, Bitmaps, "C Programs" })
                {
                    string candidate = FileSafety.Canonical(Path.Combine(Path.Combine(root, basis), relative));
                    if (FileSafety.IsWithin(candidate, root) && File.Exists(candidate))
                    {
                        screenCache[relative] = candidate;
                        return candidate;
                    }
                }
                return fallback;
            }

            private void Trim(ref int start, ref int length, bool quotes)
            {
                while (length > 0 && Char.IsWhiteSpace(text[start])) { start++; length--; }
                while (length > 0 && Char.IsWhiteSpace(text[start + length - 1])) length--;
                if (quotes && length >= 2 && text[start] == '"' && text[start + length - 1] == '"')
                {
                    start++;
                    length -= 2;
                }
            }

            private void Warn(string message)
            {
                string warning = Path.GetFileName(file) + ": " + message;
                if (!warnings.Contains(warning)) warnings.Add(warning);
            }
        }
    }
}
