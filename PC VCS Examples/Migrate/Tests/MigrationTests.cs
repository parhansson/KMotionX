using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using Migrate;

internal static class MigrationTests
{
    private static string runRoot;
    private static int passed, failed, skipped, fixtureCount;
    private static readonly UTF8Encoding Utf8 = new UTF8Encoding(false);

    private sealed class SkipTestException : Exception
    {
        internal SkipTestException(string message) : base(message) { }
    }

    private sealed class Fixture
    {
        internal readonly string Root, Old, New;
        internal Fixture()
        {
            Root = Path.Combine(runRoot, (++fixtureCount).ToString("000"));
            Old = Install(Path.Combine(Root, "KMotion4.9.0"));
            New = Install(Path.Combine(Root, "KMotion4.10.0"));
        }
        internal string OldFile(string relative, string contents)
        {
            string path = Path.Combine(Old, relative);
            Write(path, contents);
            return path;
        }
        internal string NewFile(string relative, string contents)
        {
            string path = Path.Combine(New, relative);
            Write(path, contents);
            return path;
        }
        internal string Config(string contents) { return OldFile(@"KMotion\Data\GCodeConfigCNC.txt", contents); }
        internal string Threads(string contents) { return OldFile(@"KMotion\Data\Threads.txt", contents); }
        internal string NewConfig { get { return Path.Combine(New, @"KMotion\Data\GCodeConfigCNC.txt"); } }
        internal MigrationPlan Plan() { return MigrationEngine.BuildPlan(Old, New); }
        internal MigrationResult Merge(MigrationPlan plan) { return MigrationEngine.Execute(plan, plan.OptionalFiles, null); }
    }

    private static int Main(string[] arguments)
    {
        if (arguments.Length == 4 && arguments[0] == "--preview-assembly")
        {
            var assembly = System.Reflection.Assembly.LoadFile(Path.GetFullPath(arguments[1]));
            Type engine = assembly.GetType("Migrate.MigrationEngine", true);
            object preview = engine.GetMethod("BuildPlan", new[] { typeof(string), typeof(string) }).Invoke(null, new object[] { arguments[2], arguments[3] });
            var warnings = ((System.Collections.IEnumerable)preview.GetType().GetProperty("Warnings").GetValue(preview, null)).Cast<string>().ToList();
            var probeWarnings = warnings.Where(w => w.IndexOf("ProbeInside", StringComparison.OrdinalIgnoreCase) >= 0).ToList();
            var names = probeWarnings.Select(w => System.Text.RegularExpressions.Regex.Match(w, @"ProbeInside[^\s\\/:()]*\.c", System.Text.RegularExpressions.RegexOptions.IgnoreCase).Value).Distinct(StringComparer.OrdinalIgnoreCase).OrderBy(n => n).ToArray();
            Console.WriteLine("READ-ONLY ASSEMBLY PREVIEW: " + arguments[1]);
            Console.WriteLine(warnings.Count + " total warnings; " + probeWarnings.Count + " ProbeInside warnings; " + names.Length + " distinct ProbeInside names.");
            Console.WriteLine("ProbeInside filenames: " + String.Join(", ", names));
            foreach (string warning in probeWarnings) Console.WriteLine(warning);
            return probeWarnings.Count == 0 ? 0 : 1;
        }
        // Explicit read-only harness mode: no fixture directories or migration writes.
        if (arguments.Length == 3 && arguments[0] == "--preview-only")
        {
            var preview = MigrationEngine.BuildPlan(arguments[1], arguments[2]);
            var probeWarnings = preview.Warnings.Where(w => w.IndexOf("ProbeInside", StringComparison.OrdinalIgnoreCase) >= 0).ToList();
            Console.WriteLine("READ-ONLY PREVIEW: " + preview.Documents.Keys.Count(p => Path.GetExtension(p).Equals(".scr", StringComparison.OrdinalIgnoreCase)) +
                " screens scanned; " + preview.Warnings.Count + " total warnings; " + probeWarnings.Count + " ProbeInside warnings.");
            foreach (string warning in probeWarnings) Console.WriteLine(warning);
            return probeWarnings.Count == 0 ? 0 : 1;
        }
        if (arguments.Length != 1) { Console.Error.WriteLine("Pass an isolated fixture directory."); return 2; }
        runRoot = Path.GetFullPath(arguments[0]);
        Directory.CreateDirectory(runRoot);
        Run("numeric versions and both discovery layouts", DiscoveryLayouts);
        Run("compact historical versions compare numerically", CompactVersions);
        Run("installation folder normalization", NormalizeFolders);
        Run("recursive Data merge, binary bytes, backup and empty folders", DataMerge);
        Run("absolute, relative and quoted program references with spaces", ProgramReferences);
        Run("differing optional targets offered but preserved when declined", ExistingOptional);
        Run("selected differing optional target replaced with backup", ReplaceOptional);
        Run("identical Data and references hidden and execute performs no writes", IdenticalNoOp);
        Run("same-sized different bytes remain in preview", SameSizeDifferentBytes);
        Run("identical bytes with different timestamps excluded", DifferentTimestamps);
        Run("configuration matching after path rebase excluded", RebasedConfigurationIdentical);
        Run("repeated migration with normalized C dependencies is no-op", RepeatNoOp);
        Run("optional-only plan copies selected differing program", OptionalOnly);
        Run("identical referenced program still discovers missing include", IdenticalParentMissingDependency);
        Run("selecting child hides now-identical relative parent screen", DynamicRelativeParent);
        Run("selecting child reveals changed absolute parent screen", DynamicAbsoluteParent);
        Run("selecting parent alone preserves declined child reference", DynamicParentOnly);
        Run("missing-source existing-target reference stale preview refused", ChangedDestinationOnlyReference);
        Run("relative screen asset collision rewrites to selected matching asset", ScreenAssetCollision);
        Run("optional selection refreshes effective Data differences", RefreshSelection);
        Run("initially identical source changed after preview refused", ChangedIdenticalSource);
        Run("initially identical destination changed after preview refused", ChangedIdenticalDestination);
        Run("selected and declined references retain correct installations", Selection);
        Run("Threads full and bare program paths", Threads);
        Run("external and similar-prefix paths unchanged", ExternalPaths);
        Run("missing reference warned and left unchanged", MissingReference);
        Run("quoted command executable and argument references", CommandReferences);
        Run("unquoted command gains quotes for new folder with spaces", CommandNewRootSpaces);
        Run("fallback GCodeConfigCNC.txt.bak references migrate", FallbackConfiguration);
        Run("screen and bitmap dependency closure", ScreenDependencies);
        Run("hidden control missing up/down images stay quiet", HiddenMissingImages);
        Run("visible omitted and invalid Show neighbors retain image warnings", HiddenVisibleNeighbors);
        Run("Show after bitmap whitespace and exact spans preserved", HiddenImageSpans);
        Run("existing hidden image copied while invalid unused image stays quiet", HiddenExistingImage);
        Run("hidden script and Main background warnings remain", HiddenNonImageWarnings);
        Run("real ProbeScreen hidden control image rows stay quiet", ProbeHiddenRows);
        Run("hidden-only C actions 4 5 6 stay quiet with exact spans", HiddenOnlyCPrograms);
        Run("visible config and Threads references still warn for hidden C program", HiddenProgramVisibleReferences);
        Run("same control ID visible label before or after keeps hidden C quiet", HiddenProgramVisibleDefinitionOrder);
        Run("same control ID visible label in child keeps hidden C quiet", HiddenProgramVisibleChild);
        Run("same control ID different visible program does not promote hidden C", HiddenProgramDifferentVisibleProgram);
        Run("same control ID actual visible missing C still warns", HiddenProgramSameVisibleProgram);
        Run("complete real Probe screen tree keeps hidden ProbeInside programs quiet", RealProbeScreenTree);
        Run("unrelated visible control does not activate hidden C warning", HiddenProgramUnrelatedVisible);
        Run("existing hidden C program remains offered and copied", HiddenProgramExisting);
        Run("unknown visibility standalone and other actions still warn", HiddenProgramConservativeCases);
        Run("quoted C includes and header dependency closure", IncludeDependencies);
        Run("UTF-8 BOM, non-ASCII and CRLF preserved", delegate { EncodingRoundTrip(new UTF8Encoding(true), "\r\n"); });
        Run("UTF-8 no BOM and LF preserved", delegate { EncodingRoundTrip(new UTF8Encoding(false), "\n"); });
        Run("UTF-16 LE BOM and CRLF preserved", delegate { EncodingRoundTrip(Encoding.Unicode, "\r\n"); });
        Run("UTF-16 BE BOM and LF preserved", delegate { EncodingRoundTrip(Encoding.BigEndianUnicode, "\n"); });
        Run("UTF-32 BOM preserved", delegate { EncodingRoundTrip(new UTF32Encoding(false, true), "\r\n"); });
        Run("legacy ANSI encoding preserved", AnsiEncoding);
        Run("ANSI migrates to UTF-8 BOM for Unicode destination folder", AnsiUnicodeDestination);
        Run("same installation rejected", SameRoot);
        Run("overlapping installations rejected in both directions", OverlappingRoots);
        Run("source changed after preview refused before mutation", ChangedSource);
        Run("destination changed after preview refused before mutation", ChangedDestination);
        Run("new destination appeared after preview refused", NewDestinationAppeared);
        Run("unknown optional selection rejected", InvalidSelection);
        Run("later file race rolls back replacements and new files", Rollback);
        Run("linked optional source refused", LinkedOptional);
        Run("linked destination parent refused", LinkedDestination);
        Console.WriteLine("RESULT: " + passed + " passed, " + failed + " failed, " + skipped + " skipped.");
        return failed == 0 ? 0 : 1;
    }

    private static void Run(string name, Action test)
    {
        try { test(); passed++; Console.WriteLine("PASS " + name); }
        catch (SkipTestException error) { skipped++; Console.WriteLine("SKIP " + name + ": " + error.Message); }
        catch (Exception error) { failed++; Console.WriteLine("FAIL " + name + ": " + error); }
    }

    private static string Install(string root)
    {
        Directory.CreateDirectory(Path.Combine(root, @"KMotion\Data"));
        return root;
    }
    private static void Write(string path, string contents)
    {
        Directory.CreateDirectory(Path.GetDirectoryName(path));
        File.WriteAllText(path, contents, Utf8);
    }
    private static void Assert(bool condition, string message)
    {
        if (!condition) throw new Exception(message);
    }
    private static void Equal<T>(T expected, T actual, string message)
    {
        if (!EqualityComparer<T>.Default.Equals(expected, actual))
            throw new Exception(message + " Expected: " + expected + "; actual: " + actual);
    }
    private static void Bytes(byte[] expected, byte[] actual, string message)
    {
        Assert(expected.SequenceEqual(actual), message);
    }
    private static IOException Refused(Action action)
    {
        try { action(); }
        catch (IOException error) { return error; }
        throw new Exception("Expected an IOException refusal.");
    }
    private static bool HasOptional(MigrationPlan plan, string relative)
    {
        return plan.OptionalFiles.Any(file => String.Equals(file.RelativePath, relative, StringComparison.OrdinalIgnoreCase));
    }
    private static void NoBackup(Fixture fixture)
    {
        Assert(!Directory.Exists(Path.Combine(fixture.New, "MigrateBackups")), "Refusal must occur before a backup or writes.");
    }
    private static string ActionValue(int index, int action, string value)
    {
        return "Interpreter->McodeActions[" + index + "].Action=" + action + "\r\nInterpreter->McodeActions[" + index + "].String=" + value + "\r\n";
    }

    private static void DiscoveryLayouts()
    {
        var f = new Fixture();
        string drive = Path.Combine(f.Root, "drive");
        string nine = Install(Path.Combine(drive, "KMotion4.9"));
        string ten = Install(Path.Combine(drive, "KMotion4.10"));
        string twenty = Install(Path.Combine(drive, @"KMotion\4.20.1"));
        string destination = Install(Path.Combine(drive, "KMotion9.0"));
        Install(Path.Combine(drive, "KMotionSrcKogna"));
        Directory.CreateDirectory(Path.Combine(drive, "KMotion99.0"));
        var found = InstallationDiscovery.Find(destination, drive);
        Equal(3, found.Count, "Only versioned valid previous installations should be offered.");
        Equal(twenty, found[0].Root, "Nested highest version should be first.");
        Equal(ten, found[1].Root, "4.10 must exceed 4.9.");
        Equal(nine, found[2].Root, "Older version should be last.");
    }
    private static void CompactVersions()
    {
        Version compact, dotted;
        string suffix;
        Assert(InstallationDiscovery.TryVersion("KMotion435h", out compact, out suffix), "Historical compact version should parse.");
        Equal(new Version(4, 3, 5, 0), compact, "Compact version parsing.");
        Equal("h", suffix, "Historical letter suffix retained.");
        Assert(InstallationDiscovery.TryVersion("KMotion4.10", out dotted, out suffix), "Dotted version should parse.");
        Assert(dotted > compact, "4.10 must exceed 435h (4.3.5).");
        var f = new Fixture();
        string drive = Path.Combine(f.Root, "drive");
        Install(Path.Combine(drive, "KMotion435h"));
        string newest = Install(Path.Combine(drive, "KMotion4.10"));
        Equal(newest, InstallationDiscovery.Find(f.New, drive)[0].Root, "Discovery must apply numeric comparison.");
    }
    private static void NormalizeFolders()
    {
        var f = new Fixture();
        Equal(f.Old, InstallationPaths.NormalizeRoot(f.Old), "Root accepted.");
        Equal(f.Old, InstallationPaths.NormalizeRoot(Path.Combine(f.Old, "KMotion")), "KMotion subfolder accepted.");
        Equal(f.Old, InstallationPaths.NormalizeRoot(Path.Combine(f.Old, @"KMotion\Data")), "Data subfolder accepted.");
    }
    private static void DataMerge()
    {
        var f = new Fixture();
        f.OldFile(@"KMotion\Data\settings.txt", "previous setting");
        f.NewFile(@"KMotion\Data\settings.txt", "factory setting");
        f.NewFile(@"KMotion\Data\new-only.txt", "retain me");
        string binary = f.OldFile(@"KMotion\Data\nested\binary.dat", "");
        byte[] bytes = { 0, 255, 2, 3, 128, 0, 31 };
        File.WriteAllBytes(binary, bytes);
        Directory.CreateDirectory(Path.Combine(f.Old, @"KMotion\Data\empty"));
        var result = f.Merge(f.Plan());
        Equal(2, result.FilesCopied, "All recursive Data files copied.");
        Equal("previous setting", File.ReadAllText(Path.Combine(f.New, @"KMotion\Data\settings.txt")), "Previous setting wins.");
        Equal("retain me", File.ReadAllText(Path.Combine(f.New, @"KMotion\Data\new-only.txt")), "Destination-only file retained.");
        Equal("factory setting", File.ReadAllText(Path.Combine(result.BackupDirectory, @"Original\KMotion\Data\settings.txt")), "Replaced setting backed up.");
        Bytes(bytes, File.ReadAllBytes(Path.Combine(f.New, @"KMotion\Data\nested\binary.dat")), "Binary data copied exactly.");
        Assert(Directory.Exists(Path.Combine(f.New, @"KMotion\Data\empty")), "Empty Data folder retained.");
        Assert(File.ReadAllText(Path.Combine(result.BackupDirectory, "migration.log")).Contains("SUCCESS"), "Successful migration recorded.");
    }
    private static void ProgramReferences()
    {
        var f = new Fixture();
        string absolute = f.OldFile(@"C Programs\Absolute Program.c", "int main() { return 0; }");
        f.OldFile(@"C Programs\nested\Relative Program.c", "int main() { return 1; }");
        string unknown = f.OldFile(@"Custom Files\arbitrary.dat", "custom");
        string config = ActionValue(0, 4, " \"" + absolute + "\" ") + ActionValue(1, 5, @"nested\Relative Program.c") + "customSetting=\"" + unknown + "\"\r\n";
        f.Config(config);
        var plan = f.Plan();
        Equal(3, plan.OptionalFiles.Count, "All referenced program/custom files offered.");
        f.Merge(plan);
        string expected = config.Replace(absolute, Path.Combine(f.New, @"C Programs\Absolute Program.c"))
            .Replace(unknown, Path.Combine(f.New, @"Custom Files\arbitrary.dat"));
        Equal(expected, File.ReadAllText(f.NewConfig), "Path replacements preserve quotes, spaces and other text.");
        Assert(File.Exists(Path.Combine(f.New, @"C Programs\nested\Relative Program.c")), "Preserved relative program path resolves in the new installation.");
    }
    private static void ExistingOptional()
    {
        var f = new Fixture();
        string old = f.OldFile(@"C Programs\existing.c", "old default");
        string target = f.NewFile(@"C Programs\existing.c", "new default");
        f.Config(ActionValue(0, 4, old));
        var plan = f.Plan();
        Equal(1, plan.OptionalFiles.Count, "Differing existing destination program is offered.");
        Assert(plan.OptionalFiles[0].Overwrite, "Differing optional program is marked as a replacement.");
        MigrationEngine.Execute(plan, new MigrationFile[0], null);
        Equal("new default", File.ReadAllText(target), "New version program retained.");
        Assert(File.ReadAllText(f.NewConfig).Contains(target), "Reference switched to existing new version program.");
    }
    private static void ReplaceOptional()
    {
        var f = new Fixture();
        string old = f.OldFile(@"C Programs\changed.c", "previous customized program");
        string target = f.NewFile(@"C Programs\changed.c", "new shipped program");
        f.Config(ActionValue(0, 4, old));
        var plan = f.Plan();
        Equal(1, plan.OptionalFiles.Count, "Changed program is selectable.");
        Assert(plan.OptionalFiles[0].Overwrite, "Changed program shows overwrite.");
        var result = f.Merge(plan);
        Equal("previous customized program", File.ReadAllText(target), "Selected changed program copied.");
        Equal("new shipped program", File.ReadAllText(Path.Combine(result.BackupDirectory, @"Original\C Programs\changed.c")), "Replaced optional program backed up.");
    }
    private static void IdenticalNoOp()
    {
        var f = new Fixture();
        string program = f.OldFile(@"C Programs\identical.c", "same program");
        string target = f.NewFile(@"C Programs\identical.c", "same program");
        f.OldFile(@"KMotion\Data\nested\same.dat", "same data");
        string sameData = f.NewFile(@"KMotion\Data\nested\same.dat", "same data");
        f.Config(ActionValue(0, 4, program));
        f.NewFile(@"KMotion\Data\GCodeConfigCNC.txt", ActionValue(0, 4, target));
        DateTime oldTimestamp = new DateTime(2017, 3, 4, 5, 6, 8, DateTimeKind.Utc);
        File.SetLastWriteTimeUtc(sameData, oldTimestamp);
        string[] beforeFiles = Directory.GetFiles(f.New, "*", SearchOption.AllDirectories).OrderBy(p => p).ToArray();
        string[] beforeDirectories = Directory.GetDirectories(f.New, "*", SearchOption.AllDirectories).OrderBy(p => p).ToArray();
        var plan = f.Plan();
        Equal(0, plan.DataFiles.Count, "Identical effective Data bytes hidden.");
        Equal(0, plan.OptionalFiles.Count, "Identical referenced program hidden.");
        var result = f.Merge(plan);
        Equal(0, result.FilesCopied, "No files copied for equality.");
        Equal<string>(null, result.BackupDirectory, "No backup allocated for no-op.");
        NoBackup(f);
        Assert(beforeFiles.SequenceEqual(Directory.GetFiles(f.New, "*", SearchOption.AllDirectories).OrderBy(p => p)), "No new destination files created.");
        Assert(beforeDirectories.SequenceEqual(Directory.GetDirectories(f.New, "*", SearchOption.AllDirectories).OrderBy(p => p)), "No new destination folders created.");
        Equal(oldTimestamp, File.GetLastWriteTimeUtc(sameData), "No-op retains original destination timestamp.");
    }
    private static void SameSizeDifferentBytes()
    {
        var f = new Fixture();
        f.OldFile(@"KMotion\Data\changed.bin", "ABCD");
        f.NewFile(@"KMotion\Data\changed.bin", "DCBA");
        var plan = f.Plan();
        Equal(1, plan.DataFiles.Count, "Same-size byte differences must remain visible.");
        f.Merge(plan);
        Equal("ABCD", File.ReadAllText(Path.Combine(f.New, @"KMotion\Data\changed.bin")), "Differing content copied.");
    }
    private static void DifferentTimestamps()
    {
        var f = new Fixture();
        string source = f.OldFile(@"KMotion\Data\identical.bin", "same bytes");
        string destination = f.NewFile(@"KMotion\Data\identical.bin", "same bytes");
        File.SetLastWriteTimeUtc(source, new DateTime(2021, 1, 1, 0, 0, 0, DateTimeKind.Utc));
        DateTime targetTimestamp = new DateTime(2010, 1, 1, 0, 0, 0, DateTimeKind.Utc);
        File.SetLastWriteTimeUtc(destination, targetTimestamp);
        var plan = f.Plan();
        Equal(0, plan.DataFiles.Count, "Timestamp differences alone must not produce a copy.");
        Equal(0, f.Merge(plan).FilesCopied, "Timestamp-only difference is a no-op.");
        Equal(targetTimestamp, File.GetLastWriteTimeUtc(destination), "Target timestamp untouched.");
        NoBackup(f);
    }
    private static void RebasedConfigurationIdentical()
    {
        var f = new Fixture();
        string previousProgram = f.OldFile(@"C Programs\same.c", "same");
        string newProgram = f.NewFile(@"C Programs\same.c", "same");
        string raw = ActionValue(0, 4, previousProgram);
        string effective = ActionValue(0, 4, newProgram);
        Assert(raw != effective, "Fixture has different raw path text.");
        f.Config(raw);
        f.NewFile(@"KMotion\Data\GCodeConfigCNC.txt", effective);
        var plan = f.Plan();
        Equal(0, plan.DataFiles.Count, "Compare normalized output, not raw configuration text.");
        Equal(0, plan.OptionalFiles.Count, "Equal target program hidden.");
        Equal(0, f.Merge(plan).FilesCopied, "Rebased equality produces no writes.");
        Equal(effective, File.ReadAllText(f.NewConfig), "Destination document unchanged.");
        NoBackup(f);
    }
    private static void RepeatNoOp()
    {
        var f = new Fixture();
        string program = f.OldFile(@"C Programs\custom.c", "#include \"headers/custom.h\"\r\nint main() { return VALUE; }\r\n");
        f.OldFile(@"C Programs\headers\custom.h", "#define VALUE 1\r\n");
        f.Config(ActionValue(0, 4, program));
        f.Merge(f.Plan());
        int backupsBefore = Directory.GetDirectories(Path.Combine(f.New, "MigrateBackups")).Length;
        var repeated = f.Plan();
        Equal(0, repeated.DataFiles.Count, "Migrated configuration equal on repeat.");
        Equal(0, repeated.OptionalFiles.Count, "Program with normalized include and identical header equal on repeat.");
        var result = f.Merge(repeated);
        Equal(0, result.FilesCopied, "Repeated migration no-op.");
        Equal<string>(null, result.BackupDirectory, "Repeated migration creates no new backup.");
        Equal(backupsBefore, Directory.GetDirectories(Path.Combine(f.New, "MigrateBackups")).Length, "No additional backup directory created.");
    }
    private static void OptionalOnly()
    {
        var f = new Fixture();
        string source = f.OldFile(@"C Programs\changed.c", "previous machine program");
        string target = f.NewFile(@"C Programs\changed.c", "new shipped program");
        f.Config(ActionValue(0, 4, source));
        f.NewFile(@"KMotion\Data\GCodeConfigCNC.txt", ActionValue(0, 4, target));
        var plan = f.Plan();
        Equal(0, plan.DataFiles.Count, "Configuration already equal after rebase.");
        Equal(1, plan.OptionalFiles.Count, "Differing program remains selectable without any Data changes.");
        var declined = MigrationEngine.Execute(plan, new MigrationFile[0], null);
        Equal(0, declined.FilesCopied, "Declining optional-only changes is a no-op.");
        Equal<string>(null, declined.BackupDirectory, "Declined optional-only plan creates no backup.");
        Equal("new shipped program", File.ReadAllText(target), "Declined optional-only program retained.");
        NoBackup(f);
        var result = f.Merge(plan);
        Equal(1, result.FilesCopied, "Only changed optional program copied.");
        Equal("previous machine program", File.ReadAllText(target), "Optional-only selection applied.");
        Equal("new shipped program", File.ReadAllText(Path.Combine(result.BackupDirectory, @"Original\C Programs\changed.c")), "Optional-only replacement backed up.");
    }
    private static void IdenticalParentMissingDependency()
    {
        var f = new Fixture();
        string programText = "#include \"headers/missing.h\"\r\nint main() { return VALUE; }\r\n";
        f.OldFile(@"C Programs\same.c", programText);
        f.NewFile(@"C Programs\same.c", programText);
        f.OldFile(@"C Programs\headers\missing.h", "#define VALUE 1\r\n");
        string config = ActionValue(0, 4, "same.c");
        f.Config(config);
        f.NewFile(@"KMotion\Data\GCodeConfigCNC.txt", config);
        var plan = f.Plan();
        Equal(0, plan.DataFiles.Count, "Identical relative configuration hidden.");
        Equal(2, plan.OptionalFiles.Count, "Missing header and parent fallback rewrite are initially visible.");
        var header = plan.OptionalFiles.Single(file => file.RelativePath == @"C Programs\headers\missing.h");
        MigrationEngine.RefreshDataFiles(plan, new[] { header });
        Equal(1, plan.OptionalFiles.Count, "Selecting header makes relative parent equal and hides it.");
        Assert(HasOptional(plan, @"C Programs\headers\missing.h"), "Missing header is the sole effective optional difference.");
        var result = f.Merge(plan);
        Equal(1, result.FilesCopied, "Only missing include copied.");
        Equal(programText, File.ReadAllText(Path.Combine(f.New, @"C Programs\same.c")), "Identical referencing program untouched.");
        Equal("#define VALUE 1\r\n", File.ReadAllText(Path.Combine(f.New, @"C Programs\headers\missing.h")), "New relative include resolves.");
    }
    private static void DynamicRelativeParent()
    {
        var f = new Fixture();
        const string screens = @"PC VC Examples\KMotionCNC\Screens";
        string parentText = "SScript:child.scr\r\n";
        f.OldFile(screens + @"\parent.scr", parentText);
        string newParent = f.NewFile(screens + @"\parent.scr", parentText);
        f.OldFile(screens + @"\child.scr", "Caption:child\r\n");
        f.Config("m_ScreenScriptFile=parent.scr\r\n");
        f.NewFile(@"KMotion\Data\GCodeConfigCNC.txt", "m_ScreenScriptFile=parent.scr\r\n");
        var plan = f.Plan();
        Equal(2, plan.OptionalFiles.Count, "Relative parent fallback rewrite and missing child initially offered.");
        var parent = plan.OptionalFiles.Single(file => file.RelativePath == screens + @"\parent.scr");
        var child = plan.OptionalFiles.Single(file => file.RelativePath == screens + @"\child.scr");
        MigrationEngine.RefreshDataFiles(plan, new[] { child });
        Assert(!plan.OptionalFiles.Contains(parent), "Parent hidden after selected child makes relative path valid unchanged.");
        Assert(plan.OptionalFiles.Contains(child), "Missing child remains shown.");
        MigrationEngine.RefreshDataFiles(plan, new MigrationFile[0]);
        Assert(plan.OptionalFiles.Contains(parent), "Parent returns when child selection is removed.");
        MigrationEngine.RefreshDataFiles(plan, new[] { child });
        var result = MigrationEngine.Execute(plan, new[] { child }, null);
        Equal(1, result.FilesCopied, "Only child copied; no hidden parent rewrite.");
        Equal(parentText, File.ReadAllText(newParent), "Equal existing parent preserved byte-for-byte.");
        Assert(!File.Exists(Path.Combine(result.BackupDirectory, "Original", screens, "parent.scr")), "No backup needed for untouched parent.");
    }
    private static void DynamicAbsoluteParent()
    {
        var f = new Fixture();
        const string screens = @"PC VC Examples\KMotionCNC\Screens";
        string oldChild = f.OldFile(screens + @"\child.scr", "Caption:child\r\n");
        string parentText = "SScript:" + oldChild + "\r\n";
        f.OldFile(screens + @"\parent.scr", parentText);
        string newParent = f.NewFile(screens + @"\parent.scr", parentText);
        f.Config("m_ScreenScriptFile=parent.scr\r\n");
        f.NewFile(@"KMotion\Data\GCodeConfigCNC.txt", "m_ScreenScriptFile=parent.scr\r\n");
        var plan = f.Plan();
        Equal(1, plan.OptionalFiles.Count, "Parent absolute old-child bytes initially equal; only missing child shown.");
        var child = plan.OptionalFiles.Single();
        MigrationEngine.RefreshDataFiles(plan, new[] { child });
        Equal(2, plan.OptionalFiles.Count, "Selecting child reveals required parent rebase as a visible optional change.");
        var parent = plan.OptionalFiles.Single(file => file.RelativePath == screens + @"\parent.scr");
        Assert(parent.Overwrite, "Newly revealed parent is an existing-file replacement.");
        var result = MigrationEngine.Execute(plan, new[] { child, parent }, null);
        Equal(2, result.FilesCopied, "Selected child and visible changed parent copied.");
        Equal("SScript:" + Path.Combine(f.New, screens, "child.scr") + "\r\n", File.ReadAllText(newParent), "Selected parent reference rebased to selected new child.");
        Equal(parentText, File.ReadAllText(Path.Combine(result.BackupDirectory, "Original", screens, "parent.scr")), "Changed parent original backed up.");
    }
    private static void DynamicParentOnly()
    {
        var f = new Fixture();
        const string screens = @"PC VC Examples\KMotionCNC\Screens";
        string oldChild = f.OldFile(screens + @"\child.scr", "Caption:child\r\n");
        string parentText = "SScript:child.scr\r\n";
        f.OldFile(screens + @"\parent.scr", parentText);
        string newParent = f.NewFile(screens + @"\parent.scr", parentText);
        f.Config("m_ScreenScriptFile=parent.scr\r\n");
        f.NewFile(@"KMotion\Data\GCodeConfigCNC.txt", "m_ScreenScriptFile=parent.scr\r\n");
        var plan = f.Plan();
        var parent = plan.OptionalFiles.Single(file => file.RelativePath == screens + @"\parent.scr");
        MigrationEngine.RefreshDataFiles(plan, new[] { parent });
        Assert(plan.OptionalFiles.Contains(parent), "Parent fallback rewrite remains visible when child declined.");
        var result = MigrationEngine.Execute(plan, new[] { parent }, null);
        Equal(1, result.FilesCopied, "Only selected parent copied.");
        Equal("SScript:" + oldChild + "\r\n", File.ReadAllText(newParent), "Selected parent uses existing previous child after decline.");
        Assert(!File.Exists(Path.Combine(f.New, screens, "child.scr")), "Declined child not copied.");
        Equal(parentText, File.ReadAllText(Path.Combine(result.BackupDirectory, "Original", screens, "parent.scr")), "Parent replacement backed up.");
    }
    private static void ChangedDestinationOnlyReference()
    {
        var f = new Fixture();
        string missingSource = Path.Combine(f.Old, @"C Programs\new-only.c");
        string target = f.NewFile(@"C Programs\new-only.c", "existing new version program");
        f.Config(ActionValue(0, 4, missingSource));
        f.NewFile(@"KMotion\Data\GCodeConfigCNC.txt", ActionValue(0, 4, target));
        var plan = f.Plan();
        Equal(0, plan.DataFiles.Count, "Destination-only reference makes normalized config equal.");
        Equal(0, plan.OptionalFiles.Count, "Absent previous program cannot be offered.");
        Write(target, "changed after preview");
        Refused(delegate { f.Merge(plan); });
        Equal("changed after preview", File.ReadAllText(target), "Changed new-only referenced file retained.");
        NoBackup(f);
    }
    private static void ScreenAssetCollision()
    {
        var f = new Fixture();
        const string custom = @"PC VC Examples\KMotionCNC\Screens\Custom";
        const string parentText = "BitmapFile:icon.png\r\n";
        f.OldFile(custom + @"\main.scr", parentText);
        string main = f.NewFile(custom + @"\main.scr", parentText);
        f.OldFile(custom + @"\Z\icon.png", "correct previous icon");
        string conflictingIcon = f.NewFile(custom + @"\icon.png", "different new icon");
        string config = "m_ScreenScriptFile=Custom\\main.scr\r\n";
        f.Config(config);
        f.NewFile(@"KMotion\Data\GCodeConfigCNC.txt", config);
        var plan = f.Plan();
        var selected = plan.OptionalFiles.ToArray();
        MigrationEngine.RefreshDataFiles(plan, selected);
        Assert(HasOptional(plan, custom + @"\main.scr"), "Screen rewrite stays visible because unchanged relative icon would resolve to wrong new file.");
        Assert(HasOptional(plan, custom + @"\Z\icon.png"), "Original matching nested icon offered.");
        var result = MigrationEngine.Execute(plan, selected, null);
        Equal(2, result.FilesCopied, "Matching nested asset and necessary screen rewrite copied.");
        Equal("BitmapFile:" + Path.Combine(f.New, custom, @"Z\icon.png") + "\r\n", File.ReadAllText(main), "Screen explicitly references matching selected asset, avoiding new relative collision.");
        Equal("correct previous icon", File.ReadAllText(Path.Combine(f.New, custom, @"Z\icon.png")), "Matching asset copied to mirrored relative location.");
        Equal("different new icon", File.ReadAllText(conflictingIcon), "Unrelated colliding new-version asset preserved.");
    }
    private static void RefreshSelection()
    {
        var f = new Fixture();
        string source = f.OldFile(@"C Programs\missing-in-new.c", "previous program");
        string target = Path.Combine(f.New, @"C Programs\missing-in-new.c");
        f.Config(ActionValue(0, 4, source));
        f.NewFile(@"KMotion\Data\GCodeConfigCNC.txt", ActionValue(0, 4, target));
        var plan = f.Plan();
        Equal(1, plan.OptionalFiles.Count, "Missing program offered.");
        MigrationEngine.RefreshDataFiles(plan, plan.OptionalFiles);
        Equal(0, plan.DataFiles.Count, "Selecting program leaves already-rebased config equal.");
        MigrationEngine.RefreshDataFiles(plan, new MigrationFile[0]);
        Equal(1, plan.DataFiles.Count, "Declining missing program requires config to point at previous installation.");
        MigrationEngine.RefreshDataFiles(plan, plan.OptionalFiles);
        Equal(0, plan.DataFiles.Count, "Reselecting restores effective equality.");
        var result = f.Merge(plan);
        Equal(1, result.FilesCopied, "Only selected optional file is copied.");
        Equal(ActionValue(0, 4, target), File.ReadAllText(f.NewConfig), "Config remains correctly rebased.");
    }
    private static void ChangedIdenticalSource()
    {
        var f = new Fixture();
        string source = f.OldFile(@"KMotion\Data\same.txt", "identical");
        string target = f.NewFile(@"KMotion\Data\same.txt", "identical");
        var plan = f.Plan();
        Equal(0, plan.DataFiles.Count, "Initially equal file hidden.");
        Write(source, "changed after preview");
        Refused(delegate { f.Merge(plan); });
        Equal("identical", File.ReadAllText(target), "Destination remains untouched on stale skipped source.");
        NoBackup(f);
    }
    private static void ChangedIdenticalDestination()
    {
        var f = new Fixture();
        f.OldFile(@"KMotion\Data\same.txt", "identical");
        string target = f.NewFile(@"KMotion\Data\same.txt", "identical");
        var plan = f.Plan();
        Equal(0, plan.DataFiles.Count, "Initially equal file hidden.");
        Write(target, "changed after preview");
        Refused(delegate { f.Merge(plan); });
        Equal("changed after preview", File.ReadAllText(target), "Concurrent destination update remains untouched.");
        NoBackup(f);
    }
    private static void Selection()
    {
        var f = new Fixture();
        string selectedPath = f.OldFile(@"C Programs\selected.c", "selected");
        string declinedPath = f.OldFile(@"C Programs\declined.c", "declined");
        string relativeDeclined = f.OldFile(@"C Programs\relative declined.c", "relative declined");
        f.Config(ActionValue(0, 4, selectedPath) + ActionValue(1, 4, declinedPath) + ActionValue(2, 4, "relative declined.c"));
        var plan = f.Plan();
        var selected = plan.OptionalFiles.Single(file => file.SourcePath == selectedPath);
        var result = MigrationEngine.Execute(plan, new[] { selected }, null);
        string merged = File.ReadAllText(f.NewConfig);
        Assert(merged.Contains(Path.Combine(f.New, @"C Programs\selected.c")), "Selected reference rebased.");
        Assert(merged.Contains(declinedPath), "Declined absolute reference retains old target.");
        Assert(merged.Contains(relativeDeclined), "Declined relative reference becomes old absolute path.");
        Assert(!File.Exists(Path.Combine(f.New, @"C Programs\declined.c")), "Declined file not copied.");
        Assert(!File.Exists(Path.Combine(f.New, @"C Programs\relative declined.c")), "Declined relative file not copied.");
        Assert(result.Warnings.Any(w => w.Contains("not selected")), "Remaining old dependency reported.");
    }
    private static void Threads()
    {
        var f = new Fixture();
        string full = f.OldFile(@"C Programs\Full Program.c", "full");
        f.OldFile(@"C Programs\Bare Program.c", "bare");
        f.Threads(full + "\r\n\"Bare Program.c\"\r\n\r\n");
        var plan = f.Plan();
        Equal(2, plan.OptionalFiles.Count, "Full and bare thread entries offered.");
        f.Merge(plan);
        string expected = Path.Combine(f.New, @"C Programs\Full Program.c") + "\r\n\"Bare Program.c\"\r\n\r\n";
        Equal(expected, File.ReadAllText(Path.Combine(f.New, @"KMotion\Data\Threads.txt")), "Thread paths rewritten with formatting retained.");
        Assert(File.Exists(Path.Combine(f.New, @"C Programs\Bare Program.c")), "Preserved bare thread path resolves in new C Programs.");
    }
    private static void ExternalPaths()
    {
        var f = new Fixture();
        string external = Path.Combine(f.Root, @"External\program.c");
        string prefix = Path.Combine(f.Old + "-backup", @"C Programs\program.c");
        Write(external, "external"); Write(prefix, "prefix");
        string config = ActionValue(0, 4, external) + "custom=\"" + prefix + "\"\r\n";
        f.Config(config);
        var plan = f.Plan();
        Equal(0, plan.OptionalFiles.Count, "External/similar-prefix paths are not migrated.");
        f.Merge(plan);
        Equal(config, File.ReadAllText(f.NewConfig), "External references retained exactly.");
        Equal("external", File.ReadAllText(external), "External file untouched.");
    }
    private static void MissingReference()
    {
        var f = new Fixture();
        string missing = Path.Combine(f.Old, @"C Programs\missing.c");
        string config = ActionValue(0, 4, missing);
        f.Config(config);
        var plan = f.Plan();
        Equal(0, plan.OptionalFiles.Count, "Missing source file not offered.");
        Assert(plan.Warnings.Any(w => w.Contains(missing)), "Missing source file warned.");
        f.Merge(plan);
        Equal(config, File.ReadAllText(f.NewConfig), "Missing reference unchanged.");
    }
    private static void CommandReferences()
    {
        var f = new Fixture();
        string executable = f.OldFile(@"Custom Tools\run helper.exe", "not executable, fixture bytes only");
        string argument = f.OldFile(@"Custom Files\input data.txt", "input");
        string config = ActionValue(0, 7, "\"" + executable + "\" --input \"" + argument + "\" --flag");
        f.Config(config);
        var plan = f.Plan();
        Equal(2, plan.OptionalFiles.Count, "Executable and argument file offered.");
        f.Merge(plan);
        Equal(config.Replace(f.Old + "\\", f.New + "\\"), File.ReadAllText(f.NewConfig), "Command arguments and quoting retained.");
    }
    private static void ScreenDependencies()
    {
        var f = new Fixture();
        const string screens = @"PC VC Examples\KMotionCNC\Screens";
        f.OldFile(screens + @"\custom.scr", "SScript:sub.scr,BitmapFile:Face One.png;Face Two.png\r\n");
        f.OldFile(screens + @"\sub.scr", "Caption:child screen\r\n");
        f.OldFile(screens + @"\Face One.png", "bitmap-one");
        f.OldFile(screens + @"\Face Two.png", "bitmap-two");
        f.Config("m_ScreenScriptFile=custom.scr\r\n");
        var plan = f.Plan();
        Equal(4, plan.OptionalFiles.Count, "Screen dependency closure offered.");
        f.Merge(plan);
        string screen = File.ReadAllText(Path.Combine(f.New, screens + @"\custom.scr"));
        Equal("SScript:sub.scr,BitmapFile:Face One.png;Face Two.png\r\n", screen, "Relative screen and bitmap references remain unchanged.");
        Assert(File.Exists(Path.Combine(f.New, screens + @"\sub.scr")), "Relative child screen resolves in new installation.");
        Assert(File.Exists(Path.Combine(f.New, screens + @"\Face One.png")), "Relative first bitmap resolves in new installation.");
        Assert(File.Exists(Path.Combine(f.New, screens + @"\Face Two.png")), "Relative second bitmap resolves in new installation.");
    }
    private static string ScreenFixture(Fixture f, string name, string text)
    {
        string path = f.OldFile(@"PC VC Examples\KMotionCNC\Screens\" + name, text);
        f.Config("m_ScreenScriptFile=" + name + "\r\n");
        f.Threads("");
        return path;
    }
    private static bool WarnsAbout(IEnumerable<string> warnings, string name)
    {
        return warnings.Any(warning => warning.IndexOf(name, StringComparison.OrdinalIgnoreCase) >= 0);
    }
    private static void HiddenMissingImages()
    {
        var f = new Fixture();
        const string screen = "ID:IDC_Hidden,Type:PUSHBUTTON,Show:0,BitmapFile:missing-up.png;missing-down.png,Text:Hidden,Script:\r\n";
        ScreenFixture(f, "hidden.scr", screen);
        var plan = f.Plan();
        Assert(!WarnsAbout(plan.Warnings, "missing-up.png"), "Missing unused up image does not produce preview warning.");
        Assert(!WarnsAbout(plan.Warnings, "missing-down.png"), "Missing unused down image does not produce preview warning.");
        Equal(1, plan.OptionalFiles.Count, "Only referencing screen offered; absent hidden images cannot be copied.");
        var result = f.Merge(plan);
        Assert(!WarnsAbout(result.Warnings, "missing-up.png") && !WarnsAbout(result.Warnings, "missing-down.png"), "Missing hidden images remain quiet during execution.");
        Equal(screen, File.ReadAllText(Path.Combine(f.New, @"PC VC Examples\KMotionCNC\Screens\hidden.scr")), "Hidden control and unused image text remain unchanged.");
    }
    private static void HiddenVisibleNeighbors()
    {
        var f = new Fixture();
        const string screen = "ID:IDC_Hidden,Show:0,BitmapFile:shared.png;\r\n" +
            "ID:IDC_Visible,Show:1,BitmapFile:shared.png;\r\n" +
            "ID:IDC_Omitted,BitmapFile:shared.png;omitted.png\r\n" +
            "ID:IDC_Invalid,Show:unknown,BitmapFile:invalid.png;\r\n";
        string path = ScreenFixture(f, "neighbors.scr", screen);
        var refs = ReferenceScanner.Scan(path, screen, f.Old, new List<string>());
        Equal(5, refs.Count, "All nonempty image references discovered independently.");
        Assert(refs[0].IsHiddenImage, "Only hidden record marks shared image as unused.");
        Assert(refs.Skip(1).All(reference => !reference.IsHiddenImage), "Hidden flag does not leak to neighboring records.");
        var plan = f.Plan();
        Assert(WarnsAbout(plan.Warnings, "shared.png"), "Visible/unspecified control still warns for same missing image used by hidden control.");
        Assert(WarnsAbout(plan.Warnings, "omitted.png"), "Omitted Show conservatively retains warning.");
        Assert(WarnsAbout(plan.Warnings, "invalid.png"), "Invalid Show conservatively retains warning.");
        var result = f.Merge(plan);
        Assert(WarnsAbout(result.Warnings, "shared.png") && WarnsAbout(result.Warnings, "omitted.png") && WarnsAbout(result.Warnings, "invalid.png"), "Visible and ambiguous image warnings persist in merge result.");
    }
    private static void HiddenImageSpans()
    {
        var f = new Fixture();
        const string screen = "  ID:IDC_Hidden,Type:PUSHBUTTON,BitmapFile: \"missing up.png\" ;  missing down.png  , Show : 0 ,Text:Keep  two  spaces,Script:\r\n";
        string path = ScreenFixture(f, "spans.scr", screen);
        var messages = new List<string>();
        var refs = ReferenceScanner.Scan(path, screen, f.Old, messages);
        Equal(2, refs.Count, "Both whitespace-wrapped image paths discovered.");
        Assert(refs.All(reference => reference.IsHiddenImage), "Show after BitmapFile with whitespace marks both images hidden.");
        Equal("missing up.png", screen.Substring(refs[0].Start, refs[0].Length), "Quoted up-image span excludes quotes and surrounding whitespace exactly.");
        Equal("missing down.png", screen.Substring(refs[1].Start, refs[1].Length), "Down-image span excludes whitespace exactly.");
        var plan = f.Plan();
        Equal(0, plan.Warnings.Count, "Hidden reordered record produces no missing-image warnings.");
        f.Merge(plan);
        Bytes(Utf8.GetBytes(screen), File.ReadAllBytes(Path.Combine(f.New, @"PC VC Examples\KMotionCNC\Screens\spans.scr")), "Record, quotes, whitespace and CRLF preserved byte-for-byte.");
    }
    private static void HiddenExistingImage()
    {
        var f = new Fixture();
        const string screens = @"PC VC Examples\KMotionCNC\Screens";
        const string screen = "ID:IDC_Hidden,Show:0,BitmapFile:valid-hidden.png;%UNUSED%\\missing.png,Script:\r\n" +
            "ID:IDC_Hidden2,Show:0,BitmapFile:missing-hidden.png;invalid|image.png,Script:\r\n";
        ScreenFixture(f, "valid-hidden.scr", screen);
        f.OldFile(screens + @"\valid-hidden.png", "actual hidden image bytes");
        var plan = f.Plan();
        Assert(HasOptional(plan, screens + @"\valid-hidden.png"), "Existing hidden image remains selectable for migration.");
        Equal(0, plan.Warnings.Count, "Hidden missing and unsupported unused image paths stay quiet.");
        var result = f.Merge(plan);
        Equal("actual hidden image bytes", File.ReadAllText(Path.Combine(f.New, screens, "valid-hidden.png")), "Existing hidden image is still copied.");
        Equal(0, result.Warnings.Count, "No deferred missing/invalid hidden image warning appears while copying.");
        Equal(screen, File.ReadAllText(Path.Combine(f.New, screens, "valid-hidden.scr")), "Unused invalid path text is retained.");
    }
    private static void HiddenNonImageWarnings()
    {
        var f = new Fixture();
        const string screen = "Main:CX:800,Show:0,BackBitmap:global-background.png\r\n" +
            "ID:IDC_HiddenScript,Show:0,BitmapFile:unused-hidden.png;,Script:SScript:missing-child.scr\r\n" +
            "ID:IDC_HiddenAction,Show:0,BitmapFile:;,Script:Action:4;0;0;0;0;0;missing-action.c\r\n" +
            "ID:IDC_HiddenBackground,Show:0,BackBitmap:control-background.png,BitmapFile:;\r\n";
        ScreenFixture(f, "nonimage.scr", screen);
        var plan = f.Plan();
        Assert(!WarnsAbout(plan.Warnings, "unused-hidden.png"), "Only hidden BitmapFile warning suppressed.");
        Assert(WarnsAbout(plan.Warnings, "global-background.png"), "Main background warning remains even when Main includes Show:0.");
        Assert(WarnsAbout(plan.Warnings, "missing-child.scr"), "Hidden control's missing child script still warns.");
        Assert(!WarnsAbout(plan.Warnings, "missing-action.c"), "Hidden-only control's missing C action program stays quiet.");
        Assert(WarnsAbout(plan.Warnings, "control-background.png"), "Only BitmapFile is suppressed; BackBitmap keeps warning.");
        var result = f.Merge(plan);
        Assert(WarnsAbout(result.Warnings, "global-background.png") && WarnsAbout(result.Warnings, "missing-child.scr"), "Script and main-background warnings persist in merge result.");
        Assert(!WarnsAbout(result.Warnings, "missing-action.c"), "Hidden-only C program remains quiet during copying.");
    }
    private static void ProbeHiddenRows()
    {
        // Verbatim Show:0 records read from the user's Probe_JB_Stig screen.
        // Keep the regression independent of any real installation on the test machine.
        const string screen =
            "ID:IDC_But14,Type:PUSHBUTTON,X:674,Y:86,CX:166,CY:150,Show:0,Var:-1,Style:1,BitmapFile:Images\\back_left_outside_corner.png;,Colors:;;;,Font:MS Sans Serif,FontSize:11,HotKey:-1,Bold:0,Italic:0,Text:Label,ToolTipText:,Script:\r\n" +
            "ID:IDC_But32,Type:PUSHBUTTON,X:872,Y:416,CX:166,CY:150,Show:0,Var:-1,Style:1,BitmapFile:Images\\front_middle_edge.png;,Colors:;;;,Font:MS Sans Serif,FontSize:11,HotKey:-1,Bold:0,Italic:0,Text:Label,ToolTipText:,Script:\r\n" +
            "ID:IDC_But33,Type:PUSHBUTTON,X:1070,Y:416,CX:166,CY:150,Show:0,Var:-1,Style:1,BitmapFile:Images\\front_right_outside_corner.png;,Colors:;;;,Font:MS Sans Serif,FontSize:11,HotKey:-1,Bold:0,Italic:0,Text:Label,ToolTipText:,Script:\r\n";
        var f = new Fixture();
        string path = ScreenFixture(f, "ProbeScreenCode.scr", screen);
        var refs = ReferenceScanner.Scan(path, screen, f.Old, new List<string>());
        Equal(3, refs.Count, "All three real hidden image references still scanned.");
        Assert(refs.All(reference => reference.IsHiddenImage), "Each real Show:0 probe record marks its bitmap unused.");
        var plan = f.Plan();
        Equal(0, plan.Warnings.Count, "Real hidden missing probe images produce no migration warning.");
        var result = f.Merge(plan);
        Equal(0, result.Warnings.Count, "Real hidden rows stay quiet during copying.");
        Equal(screen, File.ReadAllText(Path.Combine(f.New, @"PC VC Examples\KMotionCNC\Screens\ProbeScreenCode.scr")), "Copied real probe records remain exact.");
    }
    private static string ScreenCAction(string id, string show, int action, string program)
    {
        return "ID:" + id + ",Type:PUSHBUTTON," + (show == null ? "" : "Show:" + show + ",") +
            "BitmapFile:;,Script:Action:" + action + ";0;0;0;0;0;" + program + "\r\n";
    }
    private static void HiddenOnlyCPrograms()
    {
        var f = new Fixture();
        string screen = ScreenCAction("IDC_H4", "0", 4, "missing-four.c") +
            ScreenCAction("IDC_H5", "0", 5, " missing five.c ") +
            ScreenCAction("IDC_H6", "0", 6, " \"missing six.c\" ");
        string path = ScreenFixture(f, "hidden-programs.scr", screen);
        var refs = ReferenceScanner.Scan(path, screen, f.Old, new List<string>());
        Equal(3, refs.Count, "All hidden C action references remain scanned.");
        Assert(refs.All(reference => reference.IsHiddenProgram && !reference.IsHiddenImage), "Only program-specific hidden flag marks C actions.");
        Equal("IDC_H4", refs[0].ControlId, "Action4 retains owning control ID.");
        Equal("IDC_H5", refs[1].ControlId, "Action5 retains owning control ID.");
        Equal("IDC_H6", refs[2].ControlId, "Action6 retains owning control ID.");
        Equal("missing-four.c", screen.Substring(refs[0].Start, refs[0].Length), "Action4 source span exact.");
        Equal("missing five.c", screen.Substring(refs[1].Start, refs[1].Length), "Whitespace-wrapped Action5 source span exact.");
        Equal("missing six.c", screen.Substring(refs[2].Start, refs[2].Length), "Quoted Action6 source span exact.");
        var plan = f.Plan();
        Equal(0, plan.Warnings.Count, "Missing hidden-only .c Action4/5/6 programs produce no warnings.");
        Equal(1, plan.OptionalFiles.Count, "Only screen is offered; missing C programs cannot be copied.");
        var result = f.Merge(plan);
        Equal(0, result.Warnings.Count, "No hidden-only program warnings emerge during execution.");
        Bytes(Utf8.GetBytes(screen), File.ReadAllBytes(Path.Combine(f.New, @"PC VC Examples\KMotionCNC\Screens\hidden-programs.scr")), "Action metadata, paths, quotes, whitespace and CRLF unchanged.");
    }
    private static void HiddenProgramVisibleReferences()
    {
        var f = new Fixture();
        string screen = ScreenCAction("IDC_Hidden", "0", 4, "shared.c") + ScreenCAction("IDC_Visible", "1", 5, "shared.c");
        string path = ScreenFixture(f, "shared-program.scr", screen);
        f.Config("m_ScreenScriptFile=shared-program.scr\r\n" + ActionValue(0, 4, "shared.c"));
        f.Threads("shared.c\r\n");
        var refs = ReferenceScanner.Scan(path, screen, f.Old, new List<string>());
        Assert(refs[0].IsHiddenProgram && !refs[1].IsHiddenProgram, "A visible use of the same program retains normal classification.");
        var plan = f.Plan();
        Assert(plan.Warnings.Any(w => w.Contains("shared.c") && w.Contains("screen action")), "Visible screen use still warns for missing shared program.");
        Assert(plan.Warnings.Any(w => w.Contains("shared.c") && w.Contains("KMotion thread program")), "Threads use still warns for same missing program.");
        Assert(plan.Warnings.Any(w => w.Contains("shared.c") && w.Contains("McodeActions")), "Configuration use still warns for same missing program.");
        Assert(WarnsAbout(f.Merge(plan).Warnings, "shared.c"), "Active reference warning persists in result.");
    }
    private static void HiddenProgramVisibleDefinitionOrder()
    {
        foreach (bool visibleFirst in new[] { true, false })
        {
            var f = new Fixture();
            string hidden = ScreenCAction("IDC_Reused", "0", 4, "same-id.c");
            const string visible = "ID:IDC_Reused,Type:PUSHBUTTON,Show:1,BitmapFile:;,Text:Visible definition,Script:\r\n";
            string screen = visibleFirst ? visible + hidden : hidden + visible;
            ScreenFixture(f, "definition-order.scr", screen);
            var plan = f.Plan();
            Assert(!WarnsAbout(plan.Warnings, "same-id.c"), "Same-ID visible label without Script must not promote a different hidden C use regardless of record order.");
            Assert(!WarnsAbout(f.Merge(plan).Warnings, "same-id.c"), "Hidden-only program remains quiet when copying reused-ID visible labels.");
        }
    }
    private static void HiddenProgramVisibleChild()
    {
        var f = new Fixture();
        string screen = ScreenCAction("IDC_SharedAcrossScreens", "0", 6, "child-visible.c") + "SScript:child-definition.scr\r\n";
        ScreenFixture(f, "parent-definition.scr", screen);
        f.OldFile(@"PC VC Examples\KMotionCNC\Screens\child-definition.scr", "ID:IDC_SharedAcrossScreens,Type:PUSHBUTTON,Show:1,BitmapFile:;,Script:\r\n");
        var plan = f.Plan();
        Assert(HasOptional(plan, @"PC VC Examples\KMotionCNC\Screens\child-definition.scr"), "Referenced child definition is part of scan tree.");
        Assert(!WarnsAbout(plan.Warnings, "child-visible.c"), "Later-scanned child visible label must not promote parent hidden C use by reused ID alone.");
        Assert(!WarnsAbout(f.Merge(plan).Warnings, "child-visible.c"), "Cross-screen reused-ID label does not create hidden program warning.");
    }
    private static void HiddenProgramDifferentVisibleProgram()
    {
        var f = new Fixture();
        string screen = ScreenCAction("IDC_Reused", "0", 4, "hidden-old.c") +
            ScreenCAction("IDC_Reused", "1", 4, "different-visible.c") +
            "ID:IDC_Reused,Type:PUSHBUTTON,Show:1,Text:Only a label,Script:\r\n";
        ScreenFixture(f, "different-program.scr", screen);
        var plan = f.Plan();
        Assert(!WarnsAbout(plan.Warnings, "hidden-old.c"), "Different visible action sharing ID does not expose hidden program warning.");
        Assert(WarnsAbout(plan.Warnings, "different-visible.c"), "Actual missing visible program retains its own warning.");
        var result = f.Merge(plan);
        Assert(!WarnsAbout(result.Warnings, "hidden-old.c") && WarnsAbout(result.Warnings, "different-visible.c"), "Program visibility remains reference-specific through copy.");
    }
    private static void HiddenProgramSameVisibleProgram()
    {
        var f = new Fixture();
        string screen = ScreenCAction("IDC_Reused", "0", 4, "same-program.c") +
            ScreenCAction("IDC_Reused", "1", 4, "same-program.c");
        ScreenFixture(f, "same-program.scr", screen);
        var plan = f.Plan();
        Assert(WarnsAbout(plan.Warnings, "same-program.c"), "Actual visible reference to the same missing path still warns.");
        Assert(WarnsAbout(f.Merge(plan).Warnings, "same-program.c"), "Actual visible shared-path warning persists through copy.");
    }
    private static void RealProbeScreenTree()
    {
        const string realTree = @"C:\KMotion5.5.0\PC VC Examples\KMotionCNC\Screens\Probe_JB_Stig";
        if (!Directory.Exists(realTree)) throw new SkipTestException("Optional real Probe_JB_Stig regression dataset is unavailable.");
        var f = new Fixture();
        string fixtureTree = Path.Combine(f.Old, @"PC VC Examples\KMotionCNC\Screens\Probe_JB_Stig");
        string[] screens = Directory.GetFiles(realTree, "*.scr", SearchOption.AllDirectories);
        Assert(screens.Length > 1, "Real regression dataset includes complete referenced screen tree.");
        foreach (string source in screens)
        {
            string target = Path.Combine(fixtureTree, source.Substring(realTree.Length + 1));
            Directory.CreateDirectory(Path.GetDirectoryName(target));
            File.Copy(source, target, false);
        }
        f.Config("m_ScreenScriptFile=Probe_JB_Stig\\ProbeScreenCodeShowVars.scr\r\n");
        f.Threads("");
        var plan = f.Plan();
        int scannedScreens = plan.Documents.Keys.Count(path => Path.GetExtension(path).Equals(".scr", StringComparison.OrdinalIgnoreCase));
        Assert(scannedScreens > 1, "Migration actually traversed referenced real screen definitions.");
        Assert(plan.Documents.ContainsKey(Path.Combine(fixtureTree, "ProbeScreenExt.scr")), "Probe external screen containing hidden ProbeInside actions was scanned.");
        Assert(!WarnsAbout(plan.Warnings, "ProbeInside"), "Full real referenced tree produces no hidden ProbeInside warning despite reused visible-label control IDs.");
        Console.WriteLine("REAL TREE FIXTURE: " + screens.Length + " .scr files copied; " + scannedScreens + " referenced screens scanned; 0 ProbeInside warnings.");

        // Alter only the fixture copy to prove a real visible use restores its own warning.
        string entry = Path.Combine(fixtureTree, "ProbeScreenCodeShowVars.scr");
        File.AppendAllText(entry, "\r\n" + ScreenCAction("IDC_But25", "1", 4, @"Scripts\ProbeInsideTop.c"), Utf8);
        var visiblePlan = f.Plan();
        Assert(WarnsAbout(visiblePlan.Warnings, "ProbeInsideTop.c"), "Introduced actual visible use of same missing program still warns in full real screen tree.");
        Assert(visiblePlan.Warnings.Where(w => w.Contains("ProbeInside")).All(w => w.Contains("ProbeInsideTop.c")), "Unrelated hidden ProbeInside programs remain quiet after visible fixture insertion.");
    }
    private static void HiddenProgramUnrelatedVisible()
    {
        var f = new Fixture();
        string screen = ScreenCAction("IDC_OnlyHidden", "0", 5, "quiet.c") +
            "ID:IDC_Unrelated,Type:PUSHBUTTON,Show:1,BitmapFile:;,Script:\r\n";
        ScreenFixture(f, "unrelated.scr", screen);
        var plan = f.Plan();
        Equal(0, plan.Warnings.Count, "Unrelated visible ID does not enable hidden-only control warning.");
        Equal(0, f.Merge(plan).Warnings.Count, "Hidden-only program remains quiet while unrelated control is visible.");
    }
    private static void HiddenProgramExisting()
    {
        var f = new Fixture();
        const string screens = @"PC VC Examples\KMotionCNC\Screens";
        const string source = "int main() { return 0; }\r\n";
        ScreenFixture(f, "existing-hidden.scr", ScreenCAction("IDC_ExistingHidden", "0", 4, "valid-hidden.c"));
        f.OldFile(screens + @"\valid-hidden.c", source);
        var plan = f.Plan();
        Assert(HasOptional(plan, screens + @"\valid-hidden.c"), "Existing hidden control C program remains selectable.");
        Equal(0, plan.Warnings.Count, "Valid hidden C reference produces no missing warning.");
        f.Merge(plan);
        Equal(source, File.ReadAllText(Path.Combine(f.New, screens, "valid-hidden.c")), "Existing hidden program copied normally.");
    }
    private static void HiddenProgramConservativeCases()
    {
        var f = new Fixture();
        string screen = ScreenCAction("IDC_Omitted", null, 4, "omitted-show.c") +
            ScreenCAction("IDC_Invalid", "unknown", 5, "invalid-show.c") +
            "Action:6;0;0;0;0;0;standalone.c\r\n" +
            ScreenCAction("IDC_GCodeHidden", "0", 4, "hidden-gcode.ngc") +
            ScreenCAction("IDC_CommandHidden", "0", 7, "hidden-command.exe") +
            ScreenCAction("IDC_ScriptHidden", "0", 10, "hidden-script.scr");
        ScreenFixture(f, "cases.scr", screen);
        var plan = f.Plan();
        foreach (string name in new[] { "omitted-show.c", "invalid-show.c", "standalone.c", "hidden-gcode.ngc", "hidden-command.exe", "hidden-script.scr" })
            Assert(WarnsAbout(plan.Warnings, name), "Missing program/path must still warn for conservative case: " + name);
        var result = f.Merge(plan);
        Assert(WarnsAbout(result.Warnings, "hidden-command.exe") && WarnsAbout(result.Warnings, "hidden-script.scr"), "Other hidden action types retain warnings through copy.");
    }
    private static void CommandNewRootSpaces()
    {
        var f = new Fixture();
        string executable = f.OldFile(@"Tools\helper.exe", "non-executable fixture bytes");
        string destination = Install(Path.Combine(f.Root, "New Installation 4.10"));
        string config = ActionValue(0, 7, executable + " --flag");
        f.Config(config);
        var plan = MigrationEngine.BuildPlan(f.Old, destination);
        Equal(1, plan.OptionalFiles.Count, "Unquoted old executable offered.");
        MigrationEngine.Execute(plan, plan.OptionalFiles, null);
        string expected = config.Replace(executable, "\"" + Path.Combine(destination, @"Tools\helper.exe") + "\"");
        Equal(expected, File.ReadAllText(Path.Combine(destination, @"KMotion\Data\GCodeConfigCNC.txt")), "Relocated executable is quoted without absorbing command arguments.");
    }
    private static void FallbackConfiguration()
    {
        var f = new Fixture();
        string program = f.OldFile(@"C Programs\fallback program.c", "fallback");
        string config = ActionValue(0, 4, program);
        f.OldFile(@"KMotion\Data\GCodeConfigCNC.txt.bak", config);
        var plan = f.Plan();
        Equal(1, plan.OptionalFiles.Count, "Fallback configuration program offered.");
        f.Merge(plan);
        Equal(config.Replace(program, Path.Combine(f.New, @"C Programs\fallback program.c")),
            File.ReadAllText(Path.Combine(f.New, @"KMotion\Data\GCodeConfigCNC.txt.bak")), "Fallback configuration path rebased.");
    }
    private static void IncludeDependencies()
    {
        var f = new Fixture();
        string program = f.OldFile(@"C Programs\custom.c", "#include \"Headers/my header.h\"\r\n// #include \"ignore.h\"\r\nint main() { return 0; }\r\n");
        f.OldFile(@"C Programs\Headers\my header.h", "#include \"nested.h\"\r\n");
        f.OldFile(@"C Programs\Headers\nested.h", "#define NESTED 1\r\n");
        f.Config(ActionValue(0, 4, program));
        var plan = f.Plan();
        Equal(3, plan.OptionalFiles.Count, "Program and local nested headers offered.");
        f.Merge(plan);
        Assert(File.ReadAllText(Path.Combine(f.New, @"C Programs\custom.c")).Contains("#include \"Headers/my header.h\""), "Relative C include remains unchanged.");
        Assert(File.Exists(Path.Combine(f.New, @"C Programs\Headers\my header.h")), "Relative C include resolves in migrated folder.");
        Assert(File.Exists(Path.Combine(f.New, @"C Programs\Headers\nested.h")), "Nested relative include resolves in migrated folder.");
        Assert(!HasOptional(plan, @"C Programs\ignore.h"), "Commented include ignored.");
    }
    private static void EncodingRoundTrip(Encoding encoding, string newline)
    {
        var f = new Fixture();
        string program = f.OldFile(@"C Programs\café.c", "fixture");
        string config = "Label=café 日本語" + newline + "custom=\"" + program + "\"" + newline;
        string source = f.Config("");
        byte[] before = encoding.GetPreamble().Concat(encoding.GetBytes(config)).ToArray();
        File.WriteAllBytes(source, before);
        f.Merge(f.Plan());
        string expectedText = config.Replace(program, Path.Combine(f.New, @"C Programs\café.c"));
        byte[] expected = encoding.GetPreamble().Concat(encoding.GetBytes(expectedText)).ToArray();
        Bytes(expected, File.ReadAllBytes(f.NewConfig), "Encoding, BOM, Unicode text and newline bytes retained.");
        Bytes(before, File.ReadAllBytes(source), "Source document never changed.");
    }
    private static void AnsiEncoding()
    {
        if (Encoding.Default.CodePage == 65001) throw new SkipTestException("System default encoding is UTF-8; no ANSI fallback to exercise.");
        var f = new Fixture();
        string program = f.OldFile(@"C Programs\ansi.c", "fixture");
        string config = "Label=café\r\ncustom=" + program + "\r\n";
        string source = f.Config("");
        File.WriteAllBytes(source, Encoding.Default.GetBytes(config));
        f.Merge(f.Plan());
        Bytes(Encoding.Default.GetBytes(config.Replace(program, Path.Combine(f.New, @"C Programs\ansi.c"))), File.ReadAllBytes(f.NewConfig), "Legacy codepage bytes retained without BOM.");
    }
    private static void SameRoot()
    {
        var f = new Fixture();
        Refused(delegate { MigrationEngine.BuildPlan(f.Old, f.Old.ToUpperInvariant()); });
        NoBackup(f);
    }
    private static void AnsiUnicodeDestination()
    {
        if (Encoding.Default.CodePage == 65001) throw new SkipTestException("System default encoding is UTF-8; no ANSI fallback to exercise.");
        var f = new Fixture();
        string destination = Install(Path.Combine(f.Root, "KMotion\u65b0\u3057\u30444.10"));
        string program = f.OldFile(@"C Programs\ansi.c", "fixture");
        string config = "Label=caf\u00e9\r\ncustom=" + program + "\r\n";
        string source = f.Config("");
        byte[] original = Encoding.Default.GetBytes(config);
        File.WriteAllBytes(source, original);
        var plan = MigrationEngine.BuildPlan(f.Old, destination);
        var result = MigrationEngine.Execute(plan, plan.OptionalFiles, null);
        string expectedText = config.Replace(program, Path.Combine(destination, @"C Programs\ansi.c"));
        var utf8WithBom = new UTF8Encoding(true);
        byte[] expected = utf8WithBom.GetPreamble().Concat(utf8WithBom.GetBytes(expectedText)).ToArray();
        Bytes(expected, File.ReadAllBytes(Path.Combine(destination, @"KMotion\Data\GCodeConfigCNC.txt")), "Unrepresentable new path saved losslessly in UTF-8 with BOM.");
        Bytes(original, File.ReadAllBytes(source), "ANSI source unchanged.");
        Assert(result.Warnings.Any(w => w.Contains("UTF-8")), "Encoding conversion reported.");
    }
    private static void OverlappingRoots()
    {
        var f = new Fixture();
        string nested = Install(Path.Combine(f.Old, "NestedVersion"));
        Refused(delegate { MigrationEngine.BuildPlan(f.Old, nested); });
        Refused(delegate { MigrationEngine.BuildPlan(nested, f.Old); });
    }
    private static void ChangedSource()
    {
        var f = new Fixture();
        string source = f.OldFile(@"KMotion\Data\setting.txt", "old");
        string destination = f.NewFile(@"KMotion\Data\setting.txt", "factory");
        var plan = f.Plan(); Write(source, "changed since preview");
        Refused(delegate { f.Merge(plan); });
        Equal("factory", File.ReadAllText(destination), "Destination unchanged on source mismatch.");
        NoBackup(f);
    }
    private static void ChangedDestination()
    {
        var f = new Fixture();
        f.OldFile(@"KMotion\Data\setting.txt", "old");
        string destination = f.NewFile(@"KMotion\Data\setting.txt", "factory");
        var plan = f.Plan(); Write(destination, "user changed since preview");
        Refused(delegate { f.Merge(plan); });
        Equal("user changed since preview", File.ReadAllText(destination), "Concurrent destination update retained.");
        NoBackup(f);
    }
    private static void NewDestinationAppeared()
    {
        var f = new Fixture();
        f.OldFile(@"KMotion\Data\new.txt", "previous");
        var plan = f.Plan();
        string destination = f.NewFile(@"KMotion\Data\new.txt", "appeared");
        Refused(delegate { f.Merge(plan); });
        Equal("appeared", File.ReadAllText(destination), "New concurrent destination file retained.");
        NoBackup(f);
    }
    private static void InvalidSelection()
    {
        var f = new Fixture();
        f.OldFile(@"KMotion\Data\settings.txt", "source");
        var plan = f.Plan();
        Refused(delegate { MigrationEngine.Execute(plan, new[] { new MigrationFile() }, null); });
        NoBackup(f);
    }
    private static void Rollback()
    {
        var f = new Fixture();
        f.OldFile(@"KMotion\Data\a-replace.txt", "previous-a");
        string replaced = f.NewFile(@"KMotion\Data\a-replace.txt", "factory-a");
        f.OldFile(@"KMotion\Data\b-new.txt", "previous-b");
        string changedLater = f.OldFile(@"KMotion\Data\z-later.txt", "previous-z");
        string untouched = f.NewFile(@"KMotion\Data\destination-only.txt", "destination-only");
        var plan = f.Plan();
        bool injected = false;
        IOException error = Refused(delegate
        {
            MigrationEngine.Execute(plan, plan.OptionalFiles, delegate(string progress)
            {
                if (progress == @"Copying KMotion\Data\b-new.txt")
                {
                    Equal("previous-a", File.ReadAllText(replaced), "First replacement occurred before the injected late source race.");
                    Write(changedLater, "changed during migration"); injected = true;
                }
            });
        });
        Assert(injected, "Late-copy source race was injected.");
        Assert(error.Message.Contains("rolled back"), "Failure reports successful rollback.");
        Equal("factory-a", File.ReadAllText(replaced), "Earlier overwritten file restored.");
        Assert(!File.Exists(Path.Combine(f.New, @"KMotion\Data\b-new.txt")), "Earlier new file removed.");
        Assert(!File.Exists(Path.Combine(f.New, @"KMotion\Data\z-later.txt")), "Later failed file not created.");
        Equal("destination-only", File.ReadAllText(untouched), "Destination-only file untouched by rollback.");
        string backup = Directory.GetDirectories(Path.Combine(f.New, "MigrateBackups")).Single();
        Equal("factory-a", File.ReadAllText(Path.Combine(backup, @"Original\KMotion\Data\a-replace.txt")), "Original retained after rollback.");
        Assert(File.ReadAllText(Path.Combine(backup, "migration.log")).Contains("FAILED"), "Failed migration recorded.");
    }

    [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
    [return: MarshalAs(UnmanagedType.I1)]
    private static extern bool CreateSymbolicLink(string symbolicFileName, string targetFileName, int flags);

    private static void DirectoryLink(string link, string target)
    {
        Directory.CreateDirectory(Path.GetDirectoryName(link));
        if (!CreateSymbolicLink(link, target, 3) && !CreateSymbolicLink(link, target, 1))
            throw new SkipTestException("Windows symbolic-link creation unavailable (error " + Marshal.GetLastWin32Error() + ").");
    }
    private static void LinkedOptional()
    {
        var f = new Fixture();
        string externalDirectory = Path.Combine(f.Root, "external-programs");
        string externalFile = Path.Combine(externalDirectory, "custom.c");
        Write(externalFile, "external-original");
        string link = Path.Combine(f.Old, "Linked Programs");
        DirectoryLink(link, externalDirectory);
        try
        {
            string path = Path.Combine(link, "custom.c");
            f.Config(ActionValue(0, 4, path));
            var plan = f.Plan();
            Equal(0, plan.OptionalFiles.Count, "Linked optional source not offered.");
            Assert(plan.Warnings.Any(w => w.IndexOf("link", StringComparison.OrdinalIgnoreCase) >= 0), "Linked reference warning provided.");
            f.Merge(plan);
            Assert(!File.Exists(Path.Combine(f.New, @"Linked Programs\custom.c")), "External linked file not copied.");
            Equal("external-original", File.ReadAllText(externalFile), "External target untouched.");
        }
        finally { Directory.Delete(link); }
    }
    private static void LinkedDestination()
    {
        var f = new Fixture();
        string externalDirectory = Path.Combine(f.Root, "external-destination");
        Directory.CreateDirectory(externalDirectory);
        string link = Path.Combine(f.New, @"KMotion\Data\linked");
        DirectoryLink(link, externalDirectory);
        try
        {
            f.OldFile(@"KMotion\Data\linked\settings.txt", "must not escape");
            Refused(delegate { f.Plan(); });
            Assert(!File.Exists(Path.Combine(externalDirectory, "settings.txt")), "No write through destination linked parent.");
            NoBackup(f);
        }
        finally { Directory.Delete(link); }
    }
}
