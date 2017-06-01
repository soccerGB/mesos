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

#include <stout/strings.hpp>
#include <stout/uri.hpp>


namespace uri {

  // Returns a valid uri containing a filename.
  //
  // On Windows, the / character is replaced with \ since that's the
  // directory/file separation character. Note that / will often
  // work, but will absolutely not work if the path is a long path.
  inline std::string from_path(const std::string& filepath)
  {
#if __WINDOWS__
    return "file://" + strings::replace(filepath, "\\", "/");
#else
    return "file://" + filepath;
#endif
  }
}

#endif // __STOUT_URI_HPP__
