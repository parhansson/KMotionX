using System;
using System.Collections.Generic;
using System.IO;
using System.Text.RegularExpressions;

namespace Migrate
{
    public sealed class InstallationCandidate
    {
        public string Root { get; internal set; }
        public string DisplayName { get; internal set; }
        internal Version Version;
        internal string Suffix;
        public override string ToString() { return DisplayName; }
    }

    public static class InstallationDiscovery
    {
        public static List<InstallationCandidate> Find(string destinationRoot)
        {
            return Find(destinationRoot, @"C:\");
        }

        // Injectable search drive also makes discovery testable without touching real installs.
        internal static List<InstallationCandidate> Find(string destinationRoot, string drive)
        {
            var found = new List<InstallationCandidate>();
            var seen = new HashSet<string>(StringComparer.OrdinalIgnoreCase);
            string destination = String.IsNullOrWhiteSpace(destinationRoot) ? "" : FileSafety.Canonical(destinationRoot);
            foreach (string container in new[] { drive, Path.Combine(drive, "KMotion") })
            {
                string[] folders;
                try
                {
                    FileSafety.NoLinks(container);
                    if (!Directory.Exists(container)) continue;
                    folders = Directory.GetDirectories(container, container == drive ? "KMotion*" : "*");
                }
                catch (IOException) { continue; }
                catch (UnauthorizedAccessException) { continue; }
                foreach (string folder in folders)
                {
                    try
                    {
                        string root = InstallationPaths.NormalizeRoot(folder);
                        FileSafety.NoLinks(Path.Combine(root, "KMotion", "Data"));
                        if (String.Equals(root, destination, StringComparison.OrdinalIgnoreCase) || !seen.Add(root)) continue;
                        Version version;
                        string suffix;
                        if (!TryVersion(Path.GetFileName(root), out version, out suffix)) continue;
                        found.Add(new InstallationCandidate { Root = root, DisplayName = root, Version = version, Suffix = suffix });
                    }
                    catch (IOException) { }
                    catch (UnauthorizedAccessException) { }
                    catch (ArgumentException) { }
                }
            }
            found.Sort(delegate(InstallationCandidate a, InstallationCandidate b)
            {
                int comparison = b.Version.CompareTo(a.Version);
                if (comparison != 0) return comparison;
                // Prefer the canonical release to backup copies with the same version.
                comparison = (a.Suffix.Length == 0 ? 0 : 1).CompareTo(b.Suffix.Length == 0 ? 0 : 1);
                if (comparison != 0) return comparison;
                comparison = StringComparer.OrdinalIgnoreCase.Compare(b.Suffix, a.Suffix);
                return comparison != 0 ? comparison : StringComparer.OrdinalIgnoreCase.Compare(a.Root, b.Root);
            });
            return found;
        }

        internal static bool TryVersion(string name, out Version version, out string suffix)
        {
            version = null;
            suffix = "";
            Match match = Regex.Match(name, @"^(?:KMotion)?[vV]?(\d+(?:\.\d+){0,3})([^\d.].*)?$", RegexOptions.IgnoreCase);
            if (!match.Success) return false;
            string number = match.Groups[1].Value;
            // Older releases used names such as KMotion433 and KMotion435h.
            if (number.IndexOf('.') < 0 && number.Length == 3)
                number = number[0] + "." + number[1] + "." + number[2];
            if (number.IndexOf('.') < 0) number += ".0";
            Version parsed;
            if (!Version.TryParse(number, out parsed)) return false;
            version = new Version(parsed.Major, parsed.Minor, Math.Max(0, parsed.Build), Math.Max(0, parsed.Revision));
            suffix = match.Groups[2].Value.Trim();
            return true;
        }
    }
}
