#include "../MachinePaths.h"
#include <cassert>
#include <fstream>
#include <iostream>

int main(int argc, char **argv)
{
  assert(argc == 2);
  namespace fs = std::filesystem;
  const auto root = fs::path(argv[1]) / "laser machine";
  fs::create_directories(root / "settings/fonts");
  fs::create_directories(root / "gcode");
  std::ofstream(root / "settings/machine.json") << "{}";
  std::string output;
  const auto base = fs::canonical(root);
  assert(resolveMachinePath(root.string(), "settings/machine.json", output));
  assert(output == (base / "settings/machine.json").string());
  assert(resolveMachinePath(root.string(), "./gcode/new file.ngc", output));
  assert(output == (base / "gcode/new file.ngc").string());
  assert(resolveMachinePath(root.string(), output, output));
  assert(resolveMachinePath(root.string(), ".", output));
  assert(!resolveMachinePath(root.string(), "", output));
  assert(!resolveMachinePath(root.string(), "/etc/passwd", output));
  assert(!resolveMachinePath(root.string(), "../outside", output));
  assert(!resolveMachinePath(root.string(), "gcode/../../outside", output));
  assert(!resolveMachinePath(root.string(), base.string() + "-other/file", output));
  assert(!resolveMachinePath(root.string(), std::string(300, 'x'), output));
  fs::create_directory_symlink(fs::path(argv[1]), root / "escape");
  assert(!resolveMachinePath(root.string(), "escape/new-file", output));
  fs::create_symlink(fs::path(argv[1]) / "missing", root / "dangling");
  assert(!resolveMachinePath(root.string(), "dangling", output));
  fs::create_symlink(root / "settings/machine.json", root / "settings/fonts/leak.ttf");
  assert(!resolveMachinePath(root.string(), "settings/fonts/leak.ttf", output));
  fs::current_path("/");
  assert(resolveMachinePath(root.string(), "settings/machine.json", output));
  std::cout << "Machine path isolation tests passed\n";
}
