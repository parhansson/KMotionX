using System;
using System.IO;
using System.Security.Cryptography;

namespace Migrate
{
    internal static class FileSafety
    {
        internal static string Canonical(string path)
        {
            if (String.IsNullOrWhiteSpace(path)) throw new IOException("Select an installation folder.");
            string full = Path.GetFullPath(path.Replace('/', '\\'));
            return full.Length > Path.GetPathRoot(full).Length ? full.TrimEnd('\\') : full;
        }

        internal static bool IsWithin(string path, string root)
        {
            return Canonical(path).StartsWith(Canonical(root).TrimEnd('\\') + "\\", StringComparison.OrdinalIgnoreCase);
        }

        internal static void NoLinks(string path)
        {
            // Check every existing ancestor, including links whose targets no longer exist.
            string current = Canonical(path);
            while (!String.IsNullOrEmpty(current))
            {
                try
                {
                    if ((File.GetAttributes(current) & FileAttributes.ReparsePoint) != 0)
                        throw new IOException("Linked files/folders are not migrated: " + current);
                }
                catch (FileNotFoundException) { }
                catch (DirectoryNotFoundException) { }
                current = Path.GetDirectoryName(current);
            }
        }

        internal static string Fingerprint(string path)
        {
            NoLinks(path);
            if (Directory.Exists(path)) throw new IOException("A folder occupies a file path: " + path);
            if (!File.Exists(path)) return null;
            using (var stream = File.OpenRead(path))
            using (var hash = SHA256.Create()) return Convert.ToBase64String(hash.ComputeHash(stream));
        }

        internal static void RequireWithin(string path, string root)
        {
            if (!IsWithin(path, root)) throw new IOException("Path is outside the installation: " + path);
            NoLinks(path);
        }
    }

    public static class InstallationPaths
    {
        public static string NormalizeRoot(string path)
        {
            string root = FileSafety.Canonical(path);
            if (Directory.Exists(Path.Combine(root, "KMotion", "Data"))) return root;
            if (String.Equals(Path.GetFileName(root), "Data", StringComparison.OrdinalIgnoreCase))
                root = Path.GetDirectoryName(root);
            if (String.Equals(Path.GetFileName(root), "KMotion", StringComparison.OrdinalIgnoreCase) &&
                Directory.Exists(Path.Combine(root, "Data"))) return FileSafety.Canonical(Path.GetDirectoryName(root));
            throw new IOException("Select an installation folder containing KMotion\\Data (or its KMotion/Data folder): " + path);
        }
    }
}
