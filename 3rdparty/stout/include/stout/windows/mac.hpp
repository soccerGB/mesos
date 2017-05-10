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

#ifndef __STOUT_WINDOWS_MAC_HPP__
#define __STOUT_WINDOWS_MAC_HPP__

#include <vector>

#include "error.hpp"
#include "none.hpp"
#include "result.hpp"
#include "stringify.hpp"
#include "strings.hpp"
#include "try.hpp"

// Needed if we build 32-bit, no-op for 64-bit
#ifndef _WIN64
#define _USE_32BIT_TIME_T
#endif // _WIN64

#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")


// Network utilities.
namespace net {

// Returns the MAC address of a given link device. The link device is
// specified using its name (e.g., eth0). Returns error if the link
// device is not found. Returns none if the link device is found, but
// does not have a MAC address (e.g., loopback).
inline Result<MAC> mac(const std::string& name)
{
  DWORD result;
  ULONG size = 0;

  // Indicates whether the link device is found or not.
  bool found = false;

  // Make an initial call to GetAdaptersInfo to get structure size
  if (GetAdaptersInfo(nullptr, &size) != ERROR_BUFFER_OVERFLOW) {
    return WindowsError("Calling GetAdaptersInfo returned unexpected result");
  }

  std::vector<IP_ADAPTER_INFO> adapterInfo(size / sizeof(IP_ADAPTER_INFO));
  result = GetAdaptersInfo(
      static_cast<PIP_ADAPTER_INFO>(adapterInfo.data()),
      &size);
  if (result != NO_ERROR) {
    return WindowsError(result, "GetAdaptersInfo failed");
  }

  foreach(const IP_ADAPTER_INFO& ipAdapter, adapterInfo) {
    // Construct the MAC address in a human-readable way
    std::string macAddr;
    for (UINT i = 0; i < ipAdapter.AddressLength; i++) {
      macAddr.append(strings::format("%.2X", ipAdapter.Address[i]).get());
      if (i != (ipAdapter.AddressLength - 1)) {
        macAddr.append("-");
      }
    }

    if (!strcmp(macAddr.c_str(), name.c_str())) {
      found = true;

      // Convert from xx-xx-xx-xx-xx-xx to xx:xx:xx:xx:xx:xx
      MAC mac(MAC::parse(strings::replace(name, "-", ":")).get());

      // Ignore if the address is 0 so that the results are
      // consistent across all platforms.
      if (stringify(mac) == "00:00:00:00:00:00") {
        continue;
      }

      return mac;
    }
  }

  if (!found) {
    return Error("Cannot find the link device");
  }

  return None();
}

} // namespace net

#endif // __STOUT_WINDOWS_MAC_HPP__
