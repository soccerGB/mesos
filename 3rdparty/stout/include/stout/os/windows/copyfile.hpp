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

#ifndef __STOUT_OS_WINDOWS_COPYFILE_HPP__
#define __STOUT_OS_WINDOWS_COPYFILE_HPP__

#include <string>

#include <stout/error.hpp>
#include <stout/nothing.hpp>
#include <stout/path.hpp>
#include <stout/try.hpp>
#include <stout/windows.hpp>

#include <stout/internal/windows/longpath.hpp>


namespace os {

// Uses the `CopyFile` Windows API to perform a file copy.
// Unlike the Posix implementation, we do not need to check if the
// source or destination are directories, because `CopyFile` only
// works on files.
//
// https://msdn.microsoft.com/en-us/library/windows/desktop/aa363851(v=vs.85).aspx
//
// NOLINT(whitespace/line_length)

inline Try<Nothing> copyfile(
    const std::string& sourcePath,
    const std::string& destinationPath)
{
  // Relative paths are not allowed, as these are resolved based on
  // the current working directory and may be inconsistent.
  if (!path::absolute(sourcePath) || !path::absolute(destinationPath)) {
    return Error(
        "Relative paths are not allowed in `os::copyfile`: " +
        sourcePath + " -> " + destinationPath);
  }

  if (! ::CopyFileW(
      ::internal::windows::longpath(sourcePath).data(),
      ::internal::windows::longpath(destinationPath).data(),
      // NOTE: This allows the destination to be overwritten if the destination
      // already exists, as is the case in the POSIX version of `copyfile`.
      false)) {
    return WindowsError(
      "Failed to copy '" + sourcePath + "' to '" + destinationPath + "'");
  }

  return Nothing();
}

} // namespace os {

#endif // __STOUT_OS_WINDOWS_COPYFILE_HPP__
