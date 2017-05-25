// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License

#include <list>
#include <map>
#include <set>
#include <string>

#include <stout/fs.hpp>
#include <stout/os.hpp>
#include <stout/path.hpp>

#include <stout/os/copyfile.hpp>
#include <stout/os/getcwd.hpp>
#include <stout/os/ls.hpp>
#include <stout/os/mkdir.hpp>
#include <stout/os/stat.hpp>
#include <stout/os/touch.hpp>
#include <stout/os/write.hpp>

#include <stout/tests/utils.hpp>

using std::list;
using std::set;
using std::string;


class CopyFileTest : public TemporaryDirectoryTest {};


TEST_F(CopyFileTest, CopyWithAbsolutePath)
{
  const string tmpdir = os::getcwd();
  string fileContents = "data-in-file";

  string sourceFile = path::join(tmpdir, "source-file");
  EXPECT_SOME(os::write(sourceFile, fileContents));

  string newFile = path::join(tmpdir, "dest-file");
  EXPECT_FALSE(os::exists(newFile));

  EXPECT_SOME(os::copyfile(sourceFile, newFile));
  EXPECT_TRUE(os::exists(newFile));

  // Check contents of new file for correct data
  Try<string> sourceData = os::read(sourceFile);
  EXPECT_EQ(fileContents, sourceData.get());

  Try<string> destData = os::read(newFile);
  EXPECT_EQ(fileContents, destData.get());
}


TEST_F(CopyFileTest, CopyToDirectoryDestinationFails)
{
  const string tmpdir = os::getcwd();

  string sourceFile = path::join(tmpdir, "source-file");
  EXPECT_SOME(os::write(sourceFile, "Some file content"));

  string newFile = path::join(tmpdir, "dest-dir");
  EXPECT_FALSE(os::exists(newFile));
  EXPECT_SOME(os::mkdir(newFile));

  // Can't copy to a directory ...
  EXPECT_ERROR(os::copyfile(sourceFile, newFile));
}

TEST_F(CopyFileTest, DestinationEndsInSlashFails)
{
  // On Windows, copying to a file ending in a shash results in error:
  //   The filename, directory name, or volume label syntax is incorrect.
  // Verify identical behavior on all platforms

  const string tmpdir = os::getcwd();
  string fileContents = "data-in-file";

  string sourceFile = path::join(tmpdir, "source-file");
  EXPECT_SOME(os::write(sourceFile, fileContents));

  string newFile = path::join(tmpdir, "dest-file/");
  EXPECT_FALSE(os::exists(newFile));

  EXPECT_ERROR(os::copyfile(sourceFile, newFile));
}

TEST_F(CopyFileTest, CopyToRelativeFilenameFails)
{
  // If the destination file is relative, it's environment-dependent
  // where the file ends up. Just make sure it never works.

  const string tmpdir = os::getcwd();
  string fileContents = "data-in-file";

  string sourceFile = path::join(tmpdir, "source-file");
  EXPECT_SOME(os::write(sourceFile, fileContents));

  string newFile = "dest-file";

  EXPECT_ERROR(os::copyfile(sourceFile, newFile));
}
