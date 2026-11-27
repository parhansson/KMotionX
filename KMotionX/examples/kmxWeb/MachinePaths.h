#pragma once

#include <filesystem>
#include <string>

// HTTP file access is confined to one machine. Absolute in-root paths remain
// accepted for existing FileStatus/WebSocket clients. Do not follow symlinks.
inline bool resolveMachinePath(const std::string &root, const std::string &input,
                               std::string &output) {
  namespace fs = std::filesystem;

  const std::string pathInput = input;
  output.clear();

  if (pathInput.empty()) {
    return false;
  }

  std::error_code error;
  const auto base = fs::canonical(root, error);

  if (error) {
    return false;
  }

  fs::path requested(pathInput);
  for (const auto &part : requested) {
    if (part == "..") {
      return false;
    }
  }

  const auto candidate =
      (requested.is_absolute() ? requested : base / requested).lexically_normal();
  auto relative = candidate.lexically_relative(base);

  if (relative.empty() || *relative.begin() == "..") {
    return false;
  }

  auto current = base;
  for (const auto &part : relative) {
    current /= part;
    const auto status = fs::symlink_status(current, error);

    if (error && error != std::errc::no_such_file_or_directory) {
      return false;
    }

    if (fs::is_symlink(status)) {
      return false;
    }

    error.clear();
  }

  output = candidate.string();

  // Legacy controller buffers have a capacity of 256 bytes.
  if (output.size() >= 256) {
    output.clear();
    return false;
  }

  return true;
}
