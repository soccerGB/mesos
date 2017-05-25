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

#include <stdio.h>

#include <string>

#include <stout/error.hpp>
#include <stout/nothing.hpp>
#include <stout/os/stat.hpp>
#include <stout/stringify.hpp>
#include <stout/try.hpp>


namespace os {

// This implementation works by running the `cp` command with some
// additional conditions to ensure we copy a single file only,
// from an absolute file path to another absolute file path.
inline Try<Nothing> copyfile(
    const std::string& sourcePath,
    const std::string& destinationPath)
{
  // Neither the source nor destination may be a directory.
  if (stat::isdir(sourcePath) || stat::isdir(destinationPath)) {
    return Error(
        "The source and destination of `os::copyfile` may not be "
        "a directory: '" + sourcePath + "' -> '" + destinationPath + "'");
  }

  // A path ending in a slash implies a directory, which is not allowed.
  // This condition will be missed by `stat::isdir` if the directory
  // does not exist.
  if (sourcePath.back() == '/' || destinationPath.back() == '/') {
    return Error(
        "The source and destination of `os::copyfile` may not be "
        "a directory: '" + sourcePath + "' -> '" + destinationPath + "'");
  }

  // Relative paths are not allowed, as these are resolved based on
  // the current working directory and may be inconsistent.
  if (!path::absolute(sourcePath) || !path::absolute(destinationPath)) {
    return Error(
        "Relative paths are not allowed in `os::copyfile`: '" +
        sourcePath + "' -> '" + destinationPath + "'");
  }

  int status = os::spawn("cp", {"cp", sourcePath, destinationPath});

  if (status == -1) {
    return ErrnoError(
        "Failed to copy '" + sourcePath + "' to '" + destinationPath + "'");
  }

  if (!(WIFEXITED(status) && WEXITSTATUS(status) == 0)) {
    return Error(
        "Failed to copy '" + sourcePath + "': exited with status "
        + stringify(status));
  }

  return Nothing();
}

} // namespace os {

#endif // __STOUT_OS_POSIX_COPYFILE_HPP__
