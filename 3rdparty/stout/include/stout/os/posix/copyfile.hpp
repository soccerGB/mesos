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
#include <stout/try.hpp>


using namespace std;

namespace os {

inline Try<string> copyfile(
    const string& sourcePath,
    const string& destinationPath)
{
  // This implementation works by running the 'cp' command, and that command
  // has some odd behavior from an API perspective. Do a few pre-checks so
  // that 'cp' is a little more sane (and more importantly, consistent with
  // the Windows platform)

  // Is destination path a directory? If so, disallow.
  if (stat::isdir(destinationPath)) {
    return Error("Failed to copy to `" + destinationPath
        + "' - destination is a directory");
  }

  // On Windows, a destination path ending in "/" is invalid.
  size_t end = destinationPath.size() - 1;
  if (destinationPath[end] == '/' || destinationPath[end] == '\\') {
    return Error("Failed to copy to `" + destinationPath
        + "' - destination refers to a directory");
  }

  // If destination isn't absolute, acutal location is environment-specific.
  // Just disallow that in all cases.
  if (!path::absolute(destinationPath)) {
    return Error("Failed to copy to `" + destinationPath
        + "' - destination path is not an absolute path");
  }

  // Go ahead and copy the file using 'cp' command
  int status = os::spawn("cp", {"cp", sourcePath, destinationPath});

  if (status == -1) {
    return ErrnoError("Failed to copy '" + sourcePath + "'");
  }

  if (WEXITSTATUS(status)) {
    return Error(
        "Failed to copy '" + sourcePath + "': " + stringify(status));
  }

  LOG(INFO) << "Copied resource '" << sourcePath
            << "' to '" << destinationPath << "'";

  return destinationPath;
}

} // namespace os {

#endif // __STOUT_OS_POSIX_COPYFILE_HPP__
