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


namespace os {

inline Try<std::string> copyfile(
    const std::string& sourcePath,
    const std::string& destinationPath)
{
  // Use `CopyFile` to perform the file copy.
  //
  // UNIX implementations allow for destinationPath to be overwritten if it
  // already exists. Thus, we pass false to CopyFile (for bFailIfExists)
  // for same behavior.
  //
  // [1] https://msdn.microsoft.com/en-us/library/windows/desktop/aa363851(v=vs.85).aspx
  //
  // NOLINT(whitespace/line_length)

  // If destination isn't absolute, acutal location is environment-specific.
  // Just disallow that in all cases.
  if (!path::absolute(destinationPath)) {
    return Error("Failed to copy to `" + destinationPath
        + "' - destination path is not an absolute path");
  }

  const BOOL result = ::CopyFile(
      sourcePath.c_str(),
      destinationPath.c_str(),
      false);

  if (!result) {
    return WindowsError(
        "`os::copyfiile` failed to move file '" + sourcePath
        + "' to '" + destinationPath + "'");
  }

  LOG(INFO) << "Copied resource '" << sourcePath
            << "' to '" << destinationPath << "'";

  return destinationPath;
}

} // namespace os {

#endif // __STOUT_OS_WINDOWS_COPYFILE_HPP__
