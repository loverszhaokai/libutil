/******************************************************************************
 
  libutil
  
  Author: zhaokai
  
  Email: loverszhao@gmail.com

  Reference: chromium

  Description:

  Version: 1.0

******************************************************************************/

#include "util/regex_util.h"

#include "util/basictypes.h"

#include "third_party/gtest/include/gtest/gtest.h"

namespace util
{
// ------------------------------------------------------------
// --------------------- Update Begin -------------------------
// ------------------------------------------------------------

TEST(RegexUtilTest, RegexMatch)
{
  struct
  {
    const std::string pattern;
    const std::string input;
    const std::vector<std::string> expect_output;
    bool result;
  } cases [] = {
    { "c", "c", {"c"}, true},
    { "c", "", {}, true},
    { "(", "", {}, false},
    { "0*", "", {""}, true},
    { "0*", "0", {"0"}, true},
    { "0*", "0000", {"0000"}, true},
  };

  for (size_t i = 0; i < ARRAYSIZE_UNSAFE(cases); ++i)
  {
    std::string err_msg = "";
    std::vector<std::string> output;
    EXPECT_EQ(cases[i].result, RegexMatch(cases[i].pattern, cases[i].input, &output, &err_msg))
        << "cases[" << i << "]";
    EXPECT_EQ(cases[i].expect_output, output)
        << "cases[" << i << "]";
  }
}

TEST(RegexUtilTest, FullMatch)
{
  struct
  {
    const std::string pattern;
    const std::string input;
    bool result;
  } cases [] = {
    { "c", "c", true},
    { "c", "", false},
    { "(", "", false},
    { "^[0-9]*$", "12345", true},
    { "^[0-9]*$", "00000", true},
    { "^[0-9]*$", "1", true},
    { "\\w+([-+.]\\w+)*@\\w+([-.]\\w+)*\\.\\w+([-.]\\w+)*", "loverszhao@gmail.com", true},
    { "[0-9]{5}", "00000", true},
    { "([0-9]){5}", "00000", true},
    { "(([0-9]){5}){2}", "0000000000", true},
    { "(([0-9]){5}){2}", "0000011111", true},
    { "(([0-9]){5}){2}", "0123456789", true},
    { "[0-9]", "0", true},
    { "0*", "00000", true},
    { "0*", "", true},
    { "0?", "", true},
    { "0?", "0", true},
    { "0?", "00", false},
    { "[a-z][0-9]{2}{3}", "x000000", true},
    { "^a$", "a", true},
    { "^a$", "\na", false},
    { ".", "a", true},
    { ".", "abc", false},
    { "\.", ".", true},
    { "[.]", "a", false},
    { "[:digit:]", "1", false},
    { "[[:digit:]]", "1", true},
    { "[[:digit:]]{1,2}", "1", true},
    { "[[:digit:]]{1,2}", "99", true},
    { "^[[:digit:]]{1,2}", "1", true},
    { "^[[:digit:]]{1,2}", "99", true},
    //"^([[:digit:]]{1,2}|1[[:digit:]][[:digit:]]|2[0-4][[:digit:]]|25[0-5])\.";
    { "^([[:digit:]]{1,2}|1[[:digit:]][[:digit:]])", "99", true},
    { "^([[:digit:]]{1,2}|1[[:digit:]][[:digit:]])", "199", true},
    { "^([[:digit:]]{1,2}|1[[:digit:]][[:digit:]]|2[0-4][[:digit:]]|25[0-5])", "99", true},
    { "^([[:digit:]]{1,2}|1[[:digit:]][[:digit:]]|2[0-4][[:digit:]]|25[0-5])", "199", true},
    { "^([[:digit:]]{1,2}|1[[:digit:]][[:digit:]]|2[0-4][[:digit:]]|25[0-5])", "239", true},
    { "^([[:digit:]]{1,2}|1[[:digit:]][[:digit:]]|2[0-4][[:digit:]]|25[0-5])", "250", true},
    { "^([[:digit:]]{1,2}|1[[:digit:]][[:digit:]]|2[0-4][[:digit:]]|25[0-5])\.", "250.", true},
    { "^([\\/]?[[:alnum:]_]+)*$", "file_name", true},

    
  };

  for (size_t i = 0; i < ARRAYSIZE_UNSAFE(cases); ++i)
  {
    EXPECT_EQ(cases[i].result, FullMatch(cases[i].pattern, cases[i].input))
        << "cases[" << i << "] pattern=" << cases[i].pattern
        << " input=" << cases[i].input << std::endl;
  }
}


namespace
{

// Return true if @_mac is valid, such as "11:11:11:11:11:11"
// or "11 11 11 11 11 11" or "111111111111"
// flase otherwise
bool
ValidateMac(const std::string &_mac)
{
  const std::string pattern = "^[0-9a-fA-F]{2}([ -:][0-9a-fA-F]{2}){5}";
  return FullMatch(pattern, _mac);
}

  
// Return true if @_ip is valid, such as "192.168.4.1"
// flase otherwise
bool
ValidateIP(const std::string &_ip)
{
  const std::string pattern =
      "^([[:digit:]]{1,2}|1[[:digit:]][[:digit:]]|2[0-4][[:digit:]]|25[0-5])[.]"
      "([[:digit:]]{1,2}|1[[:digit:]][[:digit:]]|2[0-4][[:digit:]]|25[0-5])[.]"
      "([[:digit:]]{1,2}|1[[:digit:]][[:digit:]]|2[0-4][[:digit:]]|25[0-5])[.]"
      "([[:digit:]]{1,2}|1[[:digit:]][[:digit:]]|2[0-4][[:digit:]]|25[0-5])$";

  return FullMatch(pattern, _ip);
}

// Return true if @_port is valid which should between "0~65535"
// and it can be "0" or "65535"
// false otherwise
bool
ValidatePort(const std::string &_port)
{
  const std::string pattern = "^([1-9]|[1-9][[:digit:]]{1,3}|[1-6][0-5][0-5][0-3][0-5])$";
  return FullMatch(pattern, _port);
}

// Return true if @_path is valid, such as "/1/2/3" or "1/2/3/"
// false otherwise
bool
ValidatePath(const std::string &_path)
{
  //const std::string pattern = "^[a-zA-Z]:(((//(?! )[^///:*?\"<>|]+)+//?)|(//))[:space:]*$";
  //const std::string pattern = "(^[.]|^/|^[a-zA-Z])?:?/[.]+(/$)? ";
  //const std::string pattern = "(\/([0-9a-zA-Z]+))+";
  const std::string pattern = "^([\\/]?[[:alnum:]_]+)*$";
  return FullMatch(pattern, _path);
}

TEST(RegexUtilTest, ValidateMac)
{
  struct
  {
    const std::string input;
    bool result;
  } cases [] = {
    { ":11:22:33:44:55:66", false},
    { "11:22:33:44:55:66", true},
    { "11-22-33-44-55-66", true},
    { "11 22 33 44 55 66", true},
    { "112233445566", false},
    { "c", false},
    { "c", false},
    { "(", false},
  };

  for (size_t i = 0; i < ARRAYSIZE_UNSAFE(cases); ++i)
  {
    EXPECT_EQ(cases[i].result, ValidateMac(cases[i].input))
        << "cases[" << i << "] input=" << cases[i].input << std::endl;
  }
}

TEST(RegexUtilTest, ValidateIP)
{
  struct
  {
    const std::string input;
    bool result;
  } cases [] = {
    // Normal IP
    { "192.168.4.1", true},
    { "10.0.0.1", true},
    { "192.168.4.244", true},
    // Unnormal IP
    { "292.168.4.244", false},
    { "192.268.-1.244", false},
    { "192.168.4.264", false},
    { "....", false},
    { "192.168.1.", false},
    { "192 168 1 1", false},
  };

  for (size_t i = 0; i < ARRAYSIZE_UNSAFE(cases); ++i)
  {
    EXPECT_EQ(cases[i].result, ValidateIP(cases[i].input))
        << "cases[" << i << "] input=" << cases[i].input << std::endl;
  }
}

TEST(RegexUtilTest, ValidatePort)
{
  struct
  {
    const std::string input;
    bool result;
  } cases [] = {
    // Normal Port
    { "1", true},
    { "99", true},
    { "199", true},
    { "200", true},
    { "999", true},
    { "1999", true},
    { "30000", true},
    { "30001", true},
    { "60000", true},
    { "65535", true},

    // Unnormal Port
    { "0", false},
    { "-1", false},
    { "00", false},
    { "010", false},
    { "65536", false},
    { "65537", false},
    { "111111", false},
  };

  for (size_t i = 0; i < ARRAYSIZE_UNSAFE(cases); ++i)
  {
    EXPECT_EQ(cases[i].result, ValidatePort(cases[i].input))
        << "cases[" << i << "] input=" << cases[i].input << std::endl;
  }
}

TEST(RegexUtilTest, ValidatePath)
{
  struct
  {
    const std::string input;
    bool result;
  } cases [] = {
    // Normal Path
    { "1", true},    
    { "/1/2/3", true},
    { "1/2/3", true},

    // Unnormal Path
    { ".", false},
    { "..", false},
    { "/1/./", false},
    { "/1/../", false},
    { "//1/2", false},
    { "/1/...", false},
    { "/1//", false},
    { "./1/./", false},
    { "./1/../", false},
    { "./1/2/3", false},
    { "./1/2/3/", false},
    { "1/./", false},
    { "1/../", false},
  };

  for (size_t i = 0; i < ARRAYSIZE_UNSAFE(cases); ++i)
  {
    EXPECT_EQ(cases[i].result, ValidatePath(cases[i].input))
        << "cases[" << i << "] input=" << cases[i].input << std::endl;
  }
}



}


}; // namespace util
