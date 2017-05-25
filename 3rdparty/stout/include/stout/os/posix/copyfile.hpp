// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef __STOUT_OS_POSIX_COPYFILE_HPP__
#define __STOUT_OS_POSIX_COPYFILE_HPP__

#include <string>

#include <stout/error.hpp>
#include <stout/nothing.hpp>
#include <stout/path.hpp>
#include <stout/os/stat.hpp>
#include <stout/stringify.hpp>
#include <stout/try.hpp>


namespace os {

// This implementation works by running the `cp` command with some
// additional conditions to ensure we copy a single file only,
// from an absolute file path to another absolute file path.
//
// Directories are not supported as a destination path for two reasons:
// 1. No callers depended on that behavior,
// 2. Consistency with Windows implementation.
//
// Relative paths are not allowed, as these are resolved based on
// the current working directory and may be inconsistent.

inline Try<Nothing> copyfile(
    const std::string& source,
    const std::string& destination)
{
  // Neither the source nor destination may be a directory (because
  // it is a directory on disk, or because it has a trailing slash).
    if (stat::isdir(source) || stat::isdir(destination)
        || source.back() == '/' || destination.back() == '/') {
    return Error(
        "The source nor destination of `os::copyfile` may not be "
        "directories: '" + source + "' -> '" + destination + "'");
  }

  // Verify that we don't have relative paths.
  if (!path::absolute(source) || !path::absolute(destination)) {
    return Error(
        "Relative paths are not allowed in `os::copyfile`: '" +
        source + "' -> '" + destination + "'");
  }

  int status = os::spawn("cp", {"cp", source, destination});

  if (status == -1) {
    return ErrnoError(
        "Failed to copy '" + source + "' to '" + destination + "'");
  }

  if (!(WIFEXITED(status) && WEXITSTATUS(status) == 0)) {
    return Error(
        "Failed to copy '" + source + "': exited with status "
        + stringify(status));
  }

  return Nothing();
}

} // namespace os {

#endif // __STOUT_OS_POSIX_COPYFILE_HPP__
