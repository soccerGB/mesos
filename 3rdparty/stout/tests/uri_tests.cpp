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

#include <string>

#include <gtest/gtest.h>

#include <stout/uri.hpp>

using std::string;


TEST(UriTest, TestUriFromFilename)
{
  EXPECT_EQ("", uri::uriFromFilename("", uri::Scheme::FILE));
  EXPECT_EQ("file:///absolute/path/on/linux",
    uri::uriFromFilename("/absolute/path/on/linux", uri::Scheme::FILE));

#ifdef __WINDOWS__
  EXPECT_EQ("file://c:/somedir/somefile",
    uri::uriFromFilename("c:\\somedir\\somefile", uri::Scheme::FILE));
#endif // __WINDOWS__
}
