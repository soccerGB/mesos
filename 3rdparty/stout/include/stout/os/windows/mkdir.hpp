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

#ifndef __STOUT_OS_WINDOWS_MKDIR_HPP__
#define __STOUT_OS_WINDOWS_MKDIR_HPP__

#include <string>
#include <vector>

#include <stout/error.hpp>
#include <stout/nothing.hpp>
#include <stout/strings.hpp>
#include <stout/try.hpp>
#include <stout/windows.hpp>

#include <stout/os/exists.hpp>
#include <stout/os/constants.hpp>

#include <stout/internal/windows/longpath.hpp>


namespace os {

inline Try<Nothing> mkdir(const std::string& directory, bool recursive = true)
{
  if (!recursive) {
    if (os::exists(directory)) {
      return Nothing();
    }
    std::wstring longpath = ::internal::windows::longpath(directory);
    if (::CreateDirectoryW(longpath.data(), nullptr) == 0) {
      return WindowsError();
    }
  } else {
    std::vector<std::string> tokens = strings::tokenize(
        // Remove the long path prefix, if it already exists, otherwise the
        // tokenizer treats `?\` as the first part of the path.
        strings::remove(directory, os::LONGPATH_PREFIX, strings::Mode::PREFIX),
        stringify(os::PATH_SEPARATOR));

    std::string path;

    foreach (const std::string& token, tokens) {
      path += token + os::PATH_SEPARATOR;
      Try<Nothing> result = mkdir(path, false);
      if (result.isError()) {
        return result;
      }
    }
  }

  return Nothing();
}

} // namespace os {

#endif // __STOUT_OS_WINDOWS_MKDIR_HPP__
