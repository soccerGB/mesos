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

#ifndef __STOUT_URI_HPP__
#define __STOUT_URI_HPP__

#include <string>

#include <stout/path.hpp>
#include <stout/uri.hpp>


namespace uri {

  // Scheme for type of URI to generate

  enum class Scheme {
    FILE  // Scheme "file://"
  };

  /**
   * Returns a valid uri containing a filename
   */
  inline std::string uriFromFilename(const std::string& filepath, Scheme scheme)
  {
    if (filepath.empty()) {
      return "";
    }

    std::string prefix;

    switch (scheme)
    {
      case Scheme::FILE:
        prefix = "file://";
        ;;
    }

    #if __WINDOWS__
      return prefix + strings::replace(filepath, "\\", "/");
    #else
      return prefix + filepath;
    #endif
  }
}

#endif // __STOUT_URI_HPP__
