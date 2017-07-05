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

#ifndef __STOUT_WINDOWS_NET_HPP__
#define __STOUT_WINDOWS_NET_HPP__

#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")

#include <set>
#include <string>
#include <vector>

#include <stout/error.hpp>
#include <stout/foreach.hpp>
#include <stout/nothing.hpp>
#include <stout/stringify.hpp>
#include <stout/try.hpp>
#include <stout/windows.hpp>
#include <stout/windows/os.hpp>


namespace net {

// Returns the names of all the link devices in the system.
//
// NOTE: On Windows, the device names are GUID's which are not easily
// accessible via any command line tools.
//
// NOTE: This function only returns IPv4 info and does not return any
// info about the loopback interface.
inline Try<std::set<std::string>> links()
{
  DWORD result;
  ULONG size = 0;

  // Make an initial call to GetAdaptersInfo to get structure size.
  if (GetAdaptersInfo(nullptr, &size) != ERROR_BUFFER_OVERFLOW) {
    return WindowsError("Calling GetAdaptersInfo returned unexpected result");
  }

  std::set<std::string> names;
  std::vector<IP_ADAPTER_INFO> adapter_info(size / sizeof(IP_ADAPTER_INFO));
  result = GetAdaptersInfo(
      static_cast<PIP_ADAPTER_INFO>(adapter_info.data()),
      &size);

  if (result != NO_ERROR) {
    return WindowsError(result, "GetAdaptersInfo failed");
  }

  foreach (const IP_ADAPTER_INFO& ip_adapter, adapter_info) {
    names.insert(ip_adapter.AdapterName);
  }

  return names;
}


inline Try<std::string> hostname()
{
  return os::internal::nodename();
}


// Returns a `Try` of the result of attempting to set the `hostname`.
inline Try<Nothing> setHostname(const std::string& hostname)
{
  if (::SetComputerNameW(wide_stringify(hostname).data()) == 0) {
    return WindowsError();
  }

  return Nothing();
}

} // namespace net {

#endif // __STOUT_WINDOWS_NET_HPP__
