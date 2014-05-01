/******************************************************************************
 
  libutil
  
  Author: zhaokai
  
  Email: loverszhao@gmail.com

  Reference: chromium

  Description:

  Version: 1.0

******************************************************************************/

#include "util/file_util.h"

#include "util/basictypes.h"
#include "third_party/gtest/include/gtest/gtest.h"

namespace util
{

TEST(FileUtilTest, CreatePath)
{
  struct
  {
    std::string input;
    bool result;
  } cases[] = {
    {"/root", true},
    {"/which_is_not_exist", false}
  };

  for (int i = 0; i < ARRAYSIZE_UNSAFE(cases); ++i)
  {
    EXPECT_EQ(cases[i].result, PathExists(cases[i].input));
  }
}

TEST(FileUtilTest, DeletePath)
{
  // Must be carefully, path will be deleted without ask for twice
  
  
  
}

TEST(FileUtilTest, PathExists)
{
  struct
  {
    std::string input;
    bool result;
  } cases[] = {
    {"/root", true},
    {"./", true},
    {".", true},
    {"..", true},
    {"/which_is_not_exist", false},
    {"//which_is_not_exist", false},
    {"//which_is_not_exist//", false},
    {"//which_is//not_exist//", false},
  };

  for (int i = 0; i < ARRAYSIZE_UNSAFE(cases); ++i)
  {
    EXPECT_EQ(cases[i].result, PathExists(cases[i].input));
  }
}

}; // namespace util
