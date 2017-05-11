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

#ifndef __STOUT_WINDOWS_IP_HPP__
#define __STOUT_WINDOWS_IP_HPP__

#include <string>

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

inline Result<IPNetwork> IPNetwork::fromLinkDevice(
    const std::string& name,
    int family)
{
  DWORD result;
  ULONG size = 0;

  if (family != AF_INET && family != AF_INET6) {
    return Error("Unsupported family type: " + stringify(family));
  }

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

      // family doesn't make sense in Windows context; on Windows, all
      // types of adapters (Ethernet, Token Ring, Fiber, PPP, etc) support
      // the IP protocol.

      IP address = IP::parse(ipAdapter.IpAddressList.IpAddress.String).get();

      if (ipAdapter.IpAddressList.IpMask.String[0] != '\0') {
        IP netmask = IP::parse(ipAdapter.IpAddressList.IpMask.String).get();

        Try<IPNetwork> network = IPNetwork::create(address, netmask);
        if (network.isError()) {
           return Error(network.error());
        }

        return network.get();
      }

      // Note that this is the case where netmask is not specified.
      // We've seen such cases when VPN is used. In that case, a
      // default /32 prefix for IPv4 and /64 for IPv6 is used.
      int prefix = (family == AF_INET ? 32 : 64);
      Try<IPNetwork> network = IPNetwork::create(address, prefix);
      if (network.isError()) {
        return Error(network.error());
      }

      return network.get();
    }
  }

  if (!found) {
    return Error("Cannot find the link device");
  }

  return None();
}

} // namespace net {

#endif // #ifndef __STOUT_WINDOWS_IP_HPP__
