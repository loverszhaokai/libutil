/******************************************************************************
 
  libutil
  
  Author: zhaokai
  
  Email: loverszhao@gmail.com

  Reference: chromium

  Description:

  Version: 1.0

******************************************************************************/

#include "util/string_number_conversions.h"

#include <limits>

#include "third_party/gtest/include/gtest/gtest.h"

namespace util
{

TEST(StringNumberConversionsTest, StringToInt)
{
  static const struct
  {
    std::string input;
    int output;
    bool success;
  } cases[] = {
    {"0", 0, true},
    {"42", 42, true},
    {"42\x99", 42, false},
    {"\x99" "42\x99", 0, false},
    {"-2147483648", INT_MIN, true},
    {"2147483647", INT_MAX, true},
    {"", 0, false},
    {" 42", 42, false},
    {"42 ", 42, false},
    {" 42 ", 42, false},
    {"\t\n\v\f\r 42", 42, false},
    {"blah42", 0, false},
    {"42blah", 42, false},
    {"blah42blah", 0, false},
    {"-273.15", -273, false},
    {"+98.6", 98, false},
    {"--123", 0, false},
    {"++123", 0, false},
    {"-+123", 0, false},
    {"+-123", 0, false},
    {"-", 0, false},
    {"-2147483649", INT_MIN, false},
    {"-99999999999", INT_MIN, false},
    {"2147483648", INT_MAX, false},
    {"99999999999", INT_MAX, false},
  };

  for (size_t i = 0; i < ARRAYSIZE_UNSAFE(cases); ++i)
  {
    int output = 0;
    EXPECT_EQ(cases[i].success, StringToInt(cases[i].input, &output));
    EXPECT_EQ(cases[i].output, output);
  }

  // One additional test to verify that conversion of numbers in strings with
  // embedded NUL characters.  The NUL and extra data after it should be
  // interpreted as junk after the number.
  const char input[] = "6\06";
  std::string input_string(input, arraysize(input) - 1);
  int output;
  EXPECT_FALSE(StringToInt(input_string, &output));
  EXPECT_EQ(6, output);
}

TEST(StringNumberConversionsTest, StringToInt64)
{
  static const struct
  {
    std::string input;
    int64 output;
    bool success;
  } cases[] = {
    {"0", 0, true},
    {"42", 42, true},
    {"-2147483648", INT_MIN, true},
    {"2147483647", INT_MAX, true},
    {"-2147483649", GG_INT64_C(-2147483649), true},
    {"-99999999999", GG_INT64_C(-99999999999), true},
    {"2147483648", GG_INT64_C(2147483648), true},
    {"99999999999", GG_INT64_C(99999999999), true},
    {"9223372036854775807", kint64max, true},
    {"-9223372036854775808", kint64min, true},
    {"09", 9, true},
    {"-09", -9, true},
    {"", 0, false},
    {" 42", 42, false},
    {"42 ", 42, false},
    {"0x42", 0, false},
    {"\t\n\v\f\r 42", 42, false},
    {"blah42", 0, false},
    {"42blah", 42, false},
    {"blah42blah", 0, false},
    {"-273.15", -273, false},
    {"+98.6", 98, false},
    {"--123", 0, false},
    {"++123", 0, false},
    {"-+123", 0, false},
    {"+-123", 0, false},
    {"-", 0, false},
    {"-9223372036854775809", kint64min, false},
    {"-99999999999999999999", kint64min, false},
    {"9223372036854775808", kint64max, false},
    {"99999999999999999999", kint64max, false},
  };

  for (size_t i = 0; i < ARRAYSIZE_UNSAFE(cases); ++i)
  {
    int64 output = 0;
    EXPECT_EQ(cases[i].success, StringToInt64(cases[i].input, &output));
    EXPECT_EQ(cases[i].output, output);
  }

  // One additional test to verify that conversion of numbers in strings with
  // embedded NUL characters.  The NUL and extra data after it should be
  // interpreted as junk after the number.
  const char input[] = "6\06";
  std::string input_string(input, arraysize(input) - 1);
  int64 output;
  EXPECT_FALSE(StringToInt64(input_string, &output));
  EXPECT_EQ(6, output);
}

template <typename INT>
struct IntToStringTest {
  INT num;
  const char* sexpected;
  const char* uexpected;
};

TEST(StringNumberConversionsTest, IntToString)
{
  static const IntToStringTest<int> int_tests[] = {
    { 0, "0", "0" },
    { -1, "-1", "4294967295" },
    { std::numeric_limits<int>::max(), "2147483647", "2147483647" },
    { std::numeric_limits<int>::min(), "-2147483648", "2147483648" },
  };
  
  static const IntToStringTest<int64> int64_tests[] = {
    { 0, "0", "0" },
    { -1, "-1", "18446744073709551615" },
    { std::numeric_limits<int64>::max(),
      "9223372036854775807",
      "9223372036854775807", },
    { std::numeric_limits<int64>::min(),
      "-9223372036854775808",
      "9223372036854775808" },
  };

  for (size_t i = 0; i < arraysize(int_tests); ++i)
  {
    const IntToStringTest<int>* test = &int_tests[i];

    std::string result = ToString(test->num);
    EXPECT_EQ(test->sexpected, result);

    result = ToString((unsigned int)test->num);
    EXPECT_EQ(test->uexpected, result);
  }
  for (size_t i = 0; i < arraysize(int64_tests); ++i)
  {
    const IntToStringTest<int64>* test = &int64_tests[i];

    std::string result = ToString(test->num);
    EXPECT_EQ(test->sexpected, result);

    result = ToString((uint64)test->num);
    EXPECT_EQ(test->uexpected, result);
  }
}

TEST(StringNumberConversionsTest, Uint64ToString)
{
  static const struct
  {
    uint64 input;
    std::string output;
  } cases[] = {
    {0, "0"},
    {42, "42"},
    {INT_MAX, "2147483647"},
    {kuint64max, "18446744073709551615"},
  };

  for (size_t i = 0; i < ARRAYSIZE_UNSAFE(cases); ++i)
  {
    EXPECT_EQ(cases[i].output, ToString(cases[i].input));
  }
}

TEST(StringNumberConversionsTest, HexEncode)
{
  std::string hex(HexEncode(NULL, 0));
  EXPECT_EQ(hex.length(), 0U);
  unsigned char bytes[] = {0x01, 0xff, 0x02, 0xfe, 0x03, 0x80, 0x81};
  hex = HexEncode(bytes, sizeof(bytes));
  EXPECT_EQ(hex.compare("01FF02FE038081"), 0);
}

TEST(StringNumberConversionsTest, HexStringToInt)
{
  static const struct
  {
    std::string input;
    int output;
    bool success;
  } cases[] = {
    {"0", 0, true},
    {"42", 66, true},
    {"-42", -66, true},
    {"+42", 66, true},
    {"7fffffff", INT_MAX, true},
    {"80000000", INT_MIN, true},
    {"ffffffff", -1, true},
    {"DeadBeef", (int)0xdeadbeef, true},
    {"0x42", 66, true},
    {"-0x42", -66, true},
    {"+0x42", 66, true},
    {"0x7fffffff", INT_MAX, true},
    {"0x80000000", INT_MIN, true},
    {"0xffffffff", -1, true},
    {"0XDeadBeef", (int)0xdeadbeef, true},
    {"0x0f", 15, true},
    {"0f", 15, true},
    {" 45", 0x45, false},
    {"\t\n\v\f\r 0x45", 0x45, false},
    {" 45", 0x45, false},
    {"45 ", 0x45, false},
    {"45:", 0x45, false},
    {"efgh", 0xef, false},
    {"0xefgh", 0xef, false},
    {"hgfe", 0, false},
    {"100000000", -1, false},  // don't care about |output|, just |success|
    {"-", 0, false},
    {"", 0, false},
    {"0x", 0, false},
  };

  for (size_t i = 0; i < ARRAYSIZE_UNSAFE(cases); ++i)
  {
    int output = 0;
    EXPECT_EQ(cases[i].success, HexStringToInt(cases[i].input, &output));
    EXPECT_EQ(cases[i].output, output);
  }
  // One additional test to verify that conversion of numbers in strings with
  // embedded NUL characters.  The NUL and extra data after it should be
  // interpreted as junk after the number.
  const char input[] = "0xc0ffee\09";
  std::string input_string(input, arraysize(input) - 1);
  int output;
  EXPECT_FALSE(HexStringToInt(input_string, &output));
  EXPECT_EQ(0xc0ffee, output);
}

TEST(StringNumberConversionsTest, HexStringToBytes)
{
  static const struct
  {
    const std::string input;
    const char* output;
    size_t output_len;
    bool success;
  } cases[] = {
    {"", "", 0, false},
    {"0", "", 0, false},  // odd number of characters fails
    {"FF FF", "\xFF\xFF", 2, true},
    {" FF  FF ", "\xFF\xFF", 2, true},
    {" FFF  FF ", "", 0, false},
    {"7a68616f", "zhao", 4, true},
    {"7a 68 61 6f 20", "zhao ", 5, true},
    {"az", "", 0, false},
    {"00", "\0", 1, true},
    {"42", "\x42", 1, true},
    {"-42", "", 0, false},  // any non-hex value fails
    {"+42", "", 0, false},
    {"7fffffff", "\x7f\xff\xff\xff", 4, true},
    {"80000000", "\x80\0\0\0", 4, true},
    {"deadbeef", "\xde\xad\xbe\xef", 4, true},
    {"DeadBeef", "\xde\xad\xbe\xef", 4, true},
    {"0x42", "", 0, false},  // leading 0x fails (x is not hex)
    {"0f", "\xf", 1, true},
    {"45  ", "\x45", 1, true},
    {"efgh", "\xef", 1, false},
    {"", "", 0, false},
    {"0123456789ABCDEF", "\x01\x23\x45\x67\x89\xAB\xCD\xEF", 8, true},
    {"0123456789ABCDEF012345",
     "\x01\x23\x45\x67\x89\xAB\xCD\xEF\x01\x23\x45", 11, true},
  };

  for (size_t i = 0; i < ARRAYSIZE_UNSAFE(cases); ++i)
  {
    std::vector<uint8> output;
    std::vector<uint8> compare;
    EXPECT_EQ(cases[i].success, HexStringToBytes(cases[i].input, &output))
        << i << ": " << cases[i].input;
    for (size_t j = 0; j < cases[i].output_len; ++j)
      compare.push_back(static_cast<uint8>(cases[i].output[j]));
    ASSERT_EQ(output.size(), compare.size()) << i << ": " << cases[i].input;
    EXPECT_TRUE(std::equal(output.begin(), output.end(), compare.begin())) <<
        i << ": " << cases[i].input;
  }
}

TEST(StringNumberConversionsTest, HexStringToASCIIString)
{
  static const struct
  {
    const std::string input;
    const char* output;
    size_t output_len;
    bool success;
  } cases[] = {
    {"", "", 0, false},
    {"7a68616f", "zhao", 4, true},
    {"7a 68 61 6f 20", "zhao ", 5, true},
    {"7a 68 61 6f 20 6b 61 69", "zhao kai", 5, true}
  };

  for (size_t i = 0; i < ARRAYSIZE_UNSAFE(cases); ++i)
  {
    std::string result = "";
    EXPECT_EQ(cases[i].success, HexStringToASCIIString(cases[i].input, result))
        << i << ": " << cases[i].input;
    EXPECT_EQ(cases[i].output, result);
  }
}

TEST(StringNumberConversionsTest, ByteToHexString)
{
  struct
  {
    const uint8 input;
    std::string output;
    const std::string expected;
  } cases[] = {
    {0, "", "00"},
    {1, "", "01"},
    {2, "", "02"},
    {3, "", "03"},
    {253, "", "FD"},
    {254, "", "FE"},
    {255, "", "FF"},
  };

  for (size_t i = 0; i < ARRAYSIZE_UNSAFE(cases); ++i)
  {
    ByteToHexString(cases[i].input, &cases[i].output);
    EXPECT_EQ(cases[i].expected, cases[i].output);
  }
}

TEST(StringNumberConversionsTest, BytesToHexString)
{
  struct
  {
    std::vector<uint8> input_vec;
    std::string output;
    const std::string expected;
  } cases[] = {
    {{0}, "", "00"},
    {{0, 1, 2, 3}, "", "00 01 02 03"},
    {{0, 1, 2, 255}, "", "00 01 02 FF"},
    {{0, 1, 2, 254}, "", "00 01 02 FE"},
    {{252, 253, 254, 255}, "", "FC FD FE FF"}
  };

  for (size_t i = 0; i < ARRAYSIZE_UNSAFE(cases); ++i)
  {
    BytesToHexString(cases[i].input_vec, &cases[i].output);
    EXPECT_EQ(cases[i].expected, cases[i].output);
  }
}

TEST(StringNumberConversionsTest, ASCIIStringToHexString)
{
  struct
  {
    std::string input;
    std::string output;
    const std::string expected;
  } cases[] = {
    {"zhaokai", "", "7A 68 61 6F 6B 61 69"},
    {"zhao", "", "7A 68 61 6F"},
    {"", "", ""}
  };

  for (size_t i = 0; i < ARRAYSIZE_UNSAFE(cases); ++i)
  {
    ASCIIStringToHexString(cases[i].input, &cases[i].output);
    EXPECT_EQ(cases[i].expected, cases[i].output);
  }
}

TEST(StringNumberConversionsTest, UintToHexString)
{
  struct
  {
    unsigned int input;
    std::string output;
    const std::string expected;
  } cases[] = {
    {0, "", "00"},
    {1, "", "01"},
    {5, "", "05"},
    {127, "", "7F"},
    {128, "", "80"},
    {129, "", "81"},
    {255, "", "FF"},
    {256, "", "01 00"},
    {257, "", "01 01"},
    {UINT_MAX - 1, "", "FF FF FF FE"},
    {UINT_MAX, "", "FF FF FF FF"},
  };

  for (size_t i = 0; i < ARRAYSIZE_UNSAFE(cases); ++i)
  {
    UintToHexString(cases[i].input, &cases[i].output);
    EXPECT_EQ(cases[i].expected, cases[i].output);
  }
}

};
