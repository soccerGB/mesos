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

#ifndef __STOUT_OS_WINDOWS_GETCWD_HPP__
#define __STOUT_OS_WINDOWS_GETCWD_HPP__

#include <stout/stringify.hpp>
#include <stout/try.hpp>
#include <stout/windows.hpp>

#include <stout/internal/windows/longpath.hpp>


namespace os {

inline std::string getcwd()
{
  // First query for the buffer size required.
  DWORD length = ::GetCurrentDirectoryW(0, nullptr);
  std::vector<wchar_t> buffer;
  buffer.reserve(static_cast<size_t>(length));
  DWORD result = ::GetCurrentDirectoryW(length, buffer.data());
  // The allocated buffer should always be large enough, and there should be no
  // other reason that `GetCurrentDirectoryW` fails, as it does not require the
  // path to exist. (A failure here is a kernel failure.)
  //
  // NOTE: The sucess return value does not count the null terminating
  // character, in contrast to the fail return value which does count it.
  assert(result == length - 1);
  return strings::remove(stringify(std::wstring(buffer.data())),
                         os::LONGPATH_PREFIX,
                         strings::Mode::PREFIX);
}

} // namespace os {


#endif // __STOUT_OS_WINDOWS_GETCWD_HPP__
