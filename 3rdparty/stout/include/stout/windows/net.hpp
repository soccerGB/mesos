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

#include <set>
#include <string>
#include <vector>

#include <stout/error.hpp>
#include <stout/foreach.hpp>
#include <stout/format.hpp>
#include <stout/nothing.hpp>
#include <stout/try.hpp>
#include <stout/windows.hpp>

// Needed if we build 32-bit, no-op for 64-bit
#ifndef _WIN64
#define _USE_32BIT_TIME_T
#endif // _WIN64

#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")

namespace net {

// Returns the names of all the link devices in the system.
inline Try<std::set<std::string>> links()
{
  // On Windows, we return MAC address rather than the GUID. It's difficult
  // for administrator to get the interface GUID from Windows command line.
  DWORD result;
  ULONG size = 0;

  // Make an initial call to GetAdaptersInfo to get structure size
  if (GetAdaptersInfo(nullptr, &size) != ERROR_BUFFER_OVERFLOW) {
    return WindowsError("Calling GetAdaptersInfo returned unexpected result");
  }

  std::set<std::string> names;
  std::vector<IP_ADAPTER_INFO> adapterInfo(size / sizeof(IP_ADAPTER_INFO));
  result = GetAdaptersInfo(
      static_cast<PIP_ADAPTER_INFO>(adapterInfo.data()),
      &size);
  if (result != NO_ERROR) {
    return WindowsError(result, "GetAdaptersInfo failed");
  }

  foreach(const IP_ADAPTER_INFO& ipAdapter, adapterInfo) {
    std::string macAddr;
    for (UINT i = 0; i < ipAdapter.AddressLength; i++) {
      macAddr.append(strings::format("%.2X", ipAdapter.Address[i]).get());
      if (i != (ipAdapter.AddressLength - 1)) {
        macAddr.append("-");
      }
    }
    names.insert(macAddr);
  }

  return names;
}


// Returns a `Try` of the result of attempting to set the `hostname`.
inline Try<Nothing> setHostname(const std::string& hostname)
{
  if (SetComputerName(hostname.c_str()) == 0) {
    return WindowsError();
  }

  return Nothing();
}

} // namespace net {

#endif // __STOUT_WINDOWS_NET_HPP__
