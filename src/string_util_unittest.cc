/******************************************************************************
 
  libutil
  
  Author: zhaokai
  
  Email: loverszhao@gmail.com

  Reference: chromium

  Description:

  Version: 1.0

******************************************************************************/

#include "util/string_util.h"

#include "util/basictypes.h"

#include "third_party/gtest/include/gtest/gtest.h"

namespace util
{
// ------------------------------------------------------------
// --------------------- Update Begin -------------------------
// ------------------------------------------------------------

TEST(StringUtilTest, ToLowerASCII)
{
  EXPECT_EQ('c', ToLowerASCII('C'));
  EXPECT_EQ('c', ToLowerASCII('c'));
  EXPECT_EQ('2', ToLowerASCII('2'));

  EXPECT_EQ(L'c', ToLowerASCII(L'C'));
  EXPECT_EQ(L'c', ToLowerASCII(L'c'));
  EXPECT_EQ(L'2', ToLowerASCII(L'2'));
}

TEST(StringUtilTest, ToUpperASCII)
{
  EXPECT_EQ('C', ToUpperASCII('C'));
  EXPECT_EQ('C', ToUpperASCII('c'));
  EXPECT_EQ('2', ToUpperASCII('2'));

  EXPECT_EQ(L'C', ToUpperASCII(L'C'));
  EXPECT_EQ(L'C', ToUpperASCII(L'c'));
  EXPECT_EQ(L'2', ToUpperASCII(L'2'));
}


TEST(StringUtilTest, ReplaceChars)
{
  static struct TestData {
    const char* input;
    const char* replace_chars;
    const char* replace_with;
    const char* output;
    bool result;
  } cases[] = {
    { "", "", "", "", false },
    { "test", "", "", "test", false },
    { "test", "", "!", "test", false },
    { "test", "z", "!", "test", false },
    { "test", "e", "!", "t!st", true },
    { "test", "e", "!?", "t!?st", true },
    { "test", "ez", "!", "t!st", true },
    { "test", "zed", "!?", "t!?st", true },
    { "test", "t", "!?", "!?es!?", true },
    { "test", "et", "!>", "!>!>s!>", true },
    { "test", "zest", "!", "!!!!", true },
    { "test", "szt", "!", "!e!!", true },
    { "test", "t", "test", "testestest", true },
  };

  for (size_t i = 0; i < ARRAYSIZE_UNSAFE(cases); ++i)
  {
    std::string output;
    bool result = ReplaceChars(cases[i].input,
                               cases[i].replace_chars,
                               cases[i].replace_with,
                               &output);
    EXPECT_EQ(cases[i].result, result);
    EXPECT_EQ(cases[i].output, output);
  }
}

TEST(StringUtilTest, RemoveChars)
{
  const char* kRemoveChars = "-/+*";
  std::string input = "A-+bc/d!*";
  EXPECT_TRUE(RemoveChars(input, kRemoveChars, &input));
  EXPECT_EQ("Abcd!", input);

  // No characters match kRemoveChars.
  EXPECT_FALSE(RemoveChars(input, kRemoveChars, &input));
  EXPECT_EQ("Abcd!", input);

  // Empty string.
  input.clear();
  EXPECT_FALSE(RemoveChars(input, kRemoveChars, &input));
  EXPECT_EQ(std::string(), input);

  std::string _kRemoveChars = " ";
  const std::string _input = "   i love   dog ";
  std::string output = "";
  EXPECT_TRUE(RemoveChars(_input, _kRemoveChars, &output));
  EXPECT_EQ("ilovedog", output);
}

static const struct trim_case_ascii
{
  const char* input;
  const TrimPositions positions;
  const char* output;
  const TrimPositions return_value;
} trim_cases_ascii[] = {
  {" Google Video ", TRIM_LEADING, "Google Video ", TRIM_LEADING},
  {" Google Video ", TRIM_TRAILING, " Google Video", TRIM_TRAILING},
  {" Google Video ", TRIM_ALL, "Google Video", TRIM_ALL},
  {"Google Video", TRIM_ALL, "Google Video", TRIM_NONE},
  {"", TRIM_ALL, "", TRIM_NONE},
  {"  ", TRIM_LEADING, "", TRIM_LEADING},
  {"  ", TRIM_TRAILING, "", TRIM_TRAILING},
  {"  ", TRIM_ALL, "", TRIM_ALL},
  {"\t\rTest String\n", TRIM_ALL, "Test String", TRIM_ALL},
};

TEST(StringUtilTest, TrimWhitespace)
{
  std::string output_ascii;
  for (size_t i = 0; i < arraysize(trim_cases_ascii); ++i)
  {
    const trim_case_ascii& value = trim_cases_ascii[i];
    EXPECT_EQ(value.return_value,
              TrimWhitespace(value.input, value.positions, &output_ascii));
    EXPECT_EQ(value.output, output_ascii);
  }
}

static const struct collapse_case_ascii
{
  const char* input;
  const bool trim;
  const char* output;
} collapse_cases_ascii[] = {
  {" Google   Video ", false, "Google Video"},
  {"Google Video", false, "Google Video"},
  {"", false, ""},
  {"  ", false, ""},
  {"\t\rTest String\n", false, "Test String"},
  {"    Test     \n  \t String    ", false, "Test String"},
  {"   Test String", false, "Test String"},
  {"Test String    ", false, "Test String"},
  {"Test String", false, "Test String"},
  {" a \r b\n c \r\n d \t\re \t f \n ", false, "a b c d e f"},
  {"", true, ""},
  {"\n", true, ""},
  {"  \r  ", true, ""},
  {"\nFoo", true, "Foo"},
  {"\r  Foo  ", true, "Foo"},
  {" Foo bar ", true, "Foo bar"},
  {"  \tFoo  bar  \n", true, "Foo bar"},
  {" a \r b\n c \r\n d \t\re \t f \n ", true, "abcde f"},
};

TEST(StringUtilTest, CollapseWhitespaceASCII)
{
  for (size_t i = 0; i < arraysize(collapse_cases_ascii); ++i)
  {
    const collapse_case_ascii& value = collapse_cases_ascii[i];
    EXPECT_EQ(value.output, CollapseWhitespaceASCII(value.input, value.trim));
  }
}

TEST(StringUtilTest, StringToLowerASCII)
{
  static struct test_data
  {
    std::string input;
    std::string expect;
  } test_datas [] = {
    {"I LOVE DOG", "i love dog"},
    {"I LOVE DOGS", "i love dogs"},
    {" I LOVE DOG ", " i love dog "},
    {"00 AB CD EF GH AABBCC", "00 ab cd ef gh aabbcc"},
  };

  for (size_t i = 0; i < ARRAYSIZE_UNSAFE(test_datas); ++i)
  {
    EXPECT_EQ(test_datas[i].expect, StringToLowerASCII(test_datas[i].input))
        << "error msg=case[" << i << "]" << std::endl;
  }
}

TEST(StringUtilTest, StringToUpperASCII)
{
  static struct test_data
  {
    std::string input;
    std::string expect;
  } test_datas [] = {
    {"i love dog", "I LOVE DOG"},
    {"i love dogs", "I LOVE DOGS"},
    {" i love dog ", " I LOVE DOG "},
    {"00 ab cd ef gh aabbcc", "00 AB CD EF GH AABBCC"},
  };

  for (size_t i = 0; i < ARRAYSIZE_UNSAFE(test_datas); ++i)
  {
    EXPECT_EQ(test_datas[i].expect, StringToUpperASCII(test_datas[i].input))
        << "error msg=case[" << i << "]" << std::endl;
  }
}

TEST(StringUtilTest, FormatHexString)
{
  static struct
  {
    std::string input;
    std::string expect;
    bool result;
  } cases [] = {
    {"00 ", "00", true},
    {"00 00", "00 00", true},
    {"00 01", "00 01", true},
    {"00 11 22 33", "00 11 22 33", true},
    {" 00   11   22  ", "00 11 22", true},
    {"i love dog", "i love dog", false},
    {"000", "000", false},
    {"00 zz", "00 zz", false},
    {"000z", "000z", false},
  };

  for (size_t i = 0; i < ARRAYSIZE_UNSAFE(cases); ++i)
  {
    EXPECT_EQ(cases[i].result, FormatHexString(cases[i].input))
        << "cases:" << i + 1;
    EXPECT_EQ(cases[i].expect, cases[i].input)
        << "cases:" << i + 1;
  }
}

TEST(StringUtilTest, Reverse)
{
  static struct
  {
    std::string input;
    std::string output;
    const int step;
    std::string expect;
  } cases [] = {
    {"00 ", "", 2, " 00"},
    {"00 00", "", 2, "0 000"},
    {"AAABBBC", "", 3, "CBBBAAA"},
    {"AAABBBCC", "", 3, "CCBBBAAA"},
    {"AAABBBCCC", "", 3, "CCCBBBAAA"}
  };

  for (size_t i = 0; i < ARRAYSIZE_UNSAFE(cases); ++i)
  {
    Reverse(cases[i].input, &cases[i].output, cases[i].step);
    EXPECT_EQ(cases[i].expect, cases[i].output)
        << "cases:" << i + 1;
  }
}

TEST(StringUtilTest, IpToHexString)
{
  static struct
  {
    std::string input;
    std::string output;
    std::string expect;
    bool result;
  } cases [] = {
    {"111.111.111.111", "", "6F 6F 6F 6F", true},
    {"192.168.4.12 ", "", "C0 A8 04 0C", true},
    {"10.0.0.12 ", "", "0A 00 00 0C", true},
  };

  for (size_t i = 0; i < ARRAYSIZE_UNSAFE(cases); ++i)
  {
    EXPECT_EQ(cases[i].result, IpToHexString(cases[i].input, &cases[i].output));
    EXPECT_EQ(cases[i].expect, cases[i].output)
        << "cases:" << i + 1;
  }
}


TEST(StringUtilTest, HexStringToIp)
{
  static struct
  {
    std::string input;
    std::string output;
    std::string expect;
    bool result;
  } cases [] = {
    {"6F 6F 6F 6F", "", "111.111.111.111", true},
    {"C0 A8 04 0C", "", "192.168.4.12", true},
    {"0A 00 00 0C", "", "10.0.0.12", true},
  };

  for (size_t i = 0; i < ARRAYSIZE_UNSAFE(cases); ++i)
  {
    EXPECT_EQ(cases[i].result, HexStringToIp(cases[i].input, &cases[i].output));
    EXPECT_EQ(cases[i].expect, cases[i].output)
        << "cases:" << i + 1;
  }
}

// ------------------------------------------------------------
// --------------------- Update End -------------------------
// ------------------------------------------------------------

  
// ------------------------------------------------------------
// --------------------- Select Begin -------------------------
// ------------------------------------------------------------

TEST(StringUtilTest, IsAsciiWhitespace)
{
  EXPECT_TRUE(IsAsciiWhitespace(' '));
  EXPECT_TRUE(IsAsciiWhitespace('\n'));
  EXPECT_TRUE(IsAsciiWhitespace('\r'));
  EXPECT_TRUE(IsAsciiWhitespace('\t'));

  EXPECT_FALSE(IsAsciiWhitespace('\a'));
  EXPECT_FALSE(IsAsciiWhitespace('\b'));
  EXPECT_FALSE(IsAsciiWhitespace('\''));
  EXPECT_FALSE(IsAsciiWhitespace('\?'));
}

TEST(StringUtilTest, IsAsciiAlpha)
{
  for (int i = 0; i < 26; ++i)
  {
    EXPECT_TRUE(IsAsciiAlpha('a' + i));
    EXPECT_TRUE(IsAsciiAlpha('A' + i));
  }

  EXPECT_FALSE(IsAsciiAlpha('0'));
  EXPECT_FALSE(IsAsciiAlpha('9'));
  EXPECT_FALSE(IsAsciiAlpha('\a'));
  EXPECT_FALSE(IsAsciiAlpha('\b'));
  EXPECT_FALSE(IsAsciiAlpha('\r'));
}

TEST(StringUtilTest, IsAsciiDigit)
{
  for (int i = 0; i < 9; ++i)
  {
    EXPECT_TRUE(IsAsciiDigit('0' + i));
  }

  EXPECT_FALSE(IsAsciiDigit('\a'));
  EXPECT_FALSE(IsAsciiDigit('\b'));
  EXPECT_FALSE(IsAsciiDigit('\r'));
}

TEST(StringUtilTest, IsHexDigit)
{
  for (int i = 0; i < 9; ++i)
  {
    EXPECT_TRUE(IsHexDigit('0' + i));
  }

  for (int i = 0; i < 6; ++i)
  {
    EXPECT_TRUE(IsHexDigit('a' + i));
    EXPECT_TRUE(IsHexDigit('A' + i));
  }

  EXPECT_FALSE(IsHexDigit('_'));
  EXPECT_FALSE(IsHexDigit('.'));
  EXPECT_FALSE(IsHexDigit('\r'));
  EXPECT_FALSE(IsHexDigit('\?'));
}

TEST(StringUtilTest, HexDigitToInt)
{
  EXPECT_EQ(0, HexDigitToInt('0'));
  EXPECT_EQ(1, HexDigitToInt('1'));
  EXPECT_EQ(2, HexDigitToInt('2'));
  EXPECT_EQ(3, HexDigitToInt('3'));
  EXPECT_EQ(4, HexDigitToInt('4'));
  EXPECT_EQ(5, HexDigitToInt('5'));
  EXPECT_EQ(6, HexDigitToInt('6'));
  EXPECT_EQ(7, HexDigitToInt('7'));
  EXPECT_EQ(8, HexDigitToInt('8'));
  EXPECT_EQ(9, HexDigitToInt('9'));
  EXPECT_EQ(10, HexDigitToInt('A'));
  EXPECT_EQ(11, HexDigitToInt('B'));
  EXPECT_EQ(12, HexDigitToInt('C'));
  EXPECT_EQ(13, HexDigitToInt('D'));
  EXPECT_EQ(14, HexDigitToInt('E'));
  EXPECT_EQ(15, HexDigitToInt('F'));

  // Verify the lower case as well.
  EXPECT_EQ(10, HexDigitToInt('a'));
  EXPECT_EQ(11, HexDigitToInt('b'));
  EXPECT_EQ(12, HexDigitToInt('c'));
  EXPECT_EQ(13, HexDigitToInt('d'));
  EXPECT_EQ(14, HexDigitToInt('e'));
  EXPECT_EQ(15, HexDigitToInt('f'));
}


TEST(StringUtilTest, ContainsOnlyWhitespaceASCII)
{
  static const struct
  {
    const char* input;
    bool result;
  } contains_onlu_whitespace_ascii_cases[] = {
    { "", true },
    { " ", true },
    { "\n", true },
    { "\r", true },
    { "\t", true },
    { " \n", true },
    { " \r", true },
    { " \t", true },
    { " \n\n", true },
    { " \n\r", true },
    { " \n\t", true },
    { " \n\r\t", true },
    { "\a\n\r", false },
    { "i love dog", false },
    { "  i  ", false }
  };

  for (size_t i = 0; i < ARRAYSIZE_UNSAFE(contains_onlu_whitespace_ascii_cases); ++i)
  {
    EXPECT_EQ(contains_onlu_whitespace_ascii_cases[i].result,
              ContainsOnlyWhitespaceASCII(contains_onlu_whitespace_ascii_cases[i].input))
        << "case.i=" << i << std::endl;
  }  
}

TEST(StringUtilTest, ContainsOnlyChars)
{
  // Providing an empty list of characters should return false but for the empty
  // string.
  EXPECT_TRUE(ContainsOnlyChars("", ""));
  EXPECT_FALSE(ContainsOnlyChars("Hello", ""));

  EXPECT_TRUE(ContainsOnlyChars("", "1234"));
  EXPECT_TRUE(ContainsOnlyChars("1", "1234"));
  EXPECT_TRUE(ContainsOnlyChars("1", "4321"));
  EXPECT_TRUE(ContainsOnlyChars("123", "4321"));
  EXPECT_FALSE(ContainsOnlyChars("123a", "4321"));
}

TEST(StringUtilTest, LowerCaseEqualsASCII)
{
  static const struct
  {
    const char*    src_a;
    const char*    dst;
  } lowercase_cases[] = {
    { "FoO", "foo" },
    { "foo", "foo" },
    { "FOO", "foo" },
  };

  for (size_t i = 0; i < ARRAYSIZE_UNSAFE(lowercase_cases); ++i)
  {
    EXPECT_TRUE(LowerCaseEqualsASCII(lowercase_cases[i].src_a,
                                     lowercase_cases[i].dst));
  }
}

TEST(StringUtilTest, StartsWith)
{
  EXPECT_TRUE(StartsWithASCII("javascript:url", "javascript", true));
  EXPECT_FALSE(StartsWithASCII("JavaScript:url", "javascript", true));
  EXPECT_TRUE(StartsWithASCII("javascript:url", "javascript", false));
  EXPECT_TRUE(StartsWithASCII("JavaScript:url", "javascript", false));
  EXPECT_FALSE(StartsWithASCII("java", "javascript", true));
  EXPECT_FALSE(StartsWithASCII("java", "javascript", false));
  EXPECT_FALSE(StartsWithASCII("", "javascript", false));
  EXPECT_FALSE(StartsWithASCII("", "javascript", true));
  EXPECT_TRUE(StartsWithASCII("java", "", false));
  EXPECT_TRUE(StartsWithASCII("java", "", true));
}


TEST(StringUtilTest, EndsWith)
{
  EXPECT_TRUE(EndsWith("Foo.plugin", ".plugin", true));
  EXPECT_FALSE(EndsWith("Foo.Plugin", ".plugin", true));
  EXPECT_TRUE(EndsWith("Foo.plugin", ".plugin", false));
  EXPECT_TRUE(EndsWith("Foo.Plugin", ".plugin", false));
  EXPECT_FALSE(EndsWith(".plug", ".plugin", true));
  EXPECT_FALSE(EndsWith(".plug", ".plugin", false));
  EXPECT_FALSE(EndsWith("Foo.plugin Bar", ".plugin", true));
  EXPECT_FALSE(EndsWith("Foo.plugin Bar", ".plugin", false));
  EXPECT_FALSE(EndsWith("", ".plugin", false));
  EXPECT_FALSE(EndsWith("", ".plugin", true));
  EXPECT_TRUE(EndsWith("Foo.plugin", "", false));
  EXPECT_TRUE(EndsWith("Foo.plugin", "", true));
  EXPECT_TRUE(EndsWith(".plugin", ".plugin", false));
  EXPECT_TRUE(EndsWith(".plugin", ".plugin", true));
  EXPECT_TRUE(EndsWith("", "", false));
  EXPECT_TRUE(EndsWith("", "", true));
}

// Test for Tokenize
template <typename STR>
void TokenizeTest()
{
  std::vector<STR> r;
  size_t size;

  size = Tokenize(STR("This is a string"), STR(" "), &r);
  EXPECT_EQ(4U, size);
  ASSERT_EQ(4U, r.size());
  EXPECT_EQ(r[0], STR("This"));
  EXPECT_EQ(r[1], STR("is"));
  EXPECT_EQ(r[2], STR("a"));
  EXPECT_EQ(r[3], STR("string"));
  r.clear();

  size = Tokenize(STR("one,two,three"), STR(","), &r);
  EXPECT_EQ(3U, size);
  ASSERT_EQ(3U, r.size());
  EXPECT_EQ(r[0], STR("one"));
  EXPECT_EQ(r[1], STR("two"));
  EXPECT_EQ(r[2], STR("three"));
  r.clear();

  size = Tokenize(STR("one,two:three;four"), STR(",:"), &r);
  EXPECT_EQ(3U, size);
  ASSERT_EQ(3U, r.size());
  EXPECT_EQ(r[0], STR("one"));
  EXPECT_EQ(r[1], STR("two"));
  EXPECT_EQ(r[2], STR("three;four"));
  r.clear();

  size = Tokenize(STR("one,two:three;four"), STR(";,:"), &r);
  EXPECT_EQ(4U, size);
  ASSERT_EQ(4U, r.size());
  EXPECT_EQ(r[0], STR("one"));
  EXPECT_EQ(r[1], STR("two"));
  EXPECT_EQ(r[2], STR("three"));
  EXPECT_EQ(r[3], STR("four"));
  r.clear();

  size = Tokenize(STR("one, two, three"), STR(","), &r);
  EXPECT_EQ(3U, size);
  ASSERT_EQ(3U, r.size());
  EXPECT_EQ(r[0], STR("one"));
  EXPECT_EQ(r[1], STR(" two"));
  EXPECT_EQ(r[2], STR(" three"));
  r.clear();

  size = Tokenize(STR("one, two, three, "), STR(","), &r);
  EXPECT_EQ(4U, size);
  ASSERT_EQ(4U, r.size());
  EXPECT_EQ(r[0], STR("one"));
  EXPECT_EQ(r[1], STR(" two"));
  EXPECT_EQ(r[2], STR(" three"));
  EXPECT_EQ(r[3], STR(" "));
  r.clear();

  size = Tokenize(STR("one, two, three,"), STR(","), &r);
  EXPECT_EQ(3U, size);
  ASSERT_EQ(3U, r.size());
  EXPECT_EQ(r[0], STR("one"));
  EXPECT_EQ(r[1], STR(" two"));
  EXPECT_EQ(r[2], STR(" three"));
  r.clear();

  size = Tokenize(STR(""), STR(","), &r);
  EXPECT_EQ(0U, size);
  ASSERT_EQ(0U, r.size());
  r.clear();

  size = Tokenize(STR(","), STR(","), &r);
  EXPECT_EQ(0U, size);
  ASSERT_EQ(0U, r.size());
  r.clear();

  size = Tokenize(STR(",;:."), STR(".:;,"), &r);
  EXPECT_EQ(0U, size);
  ASSERT_EQ(0U, r.size());
  r.clear();

  size = Tokenize(STR("\t\ta\t"), STR("\t"), &r);
  EXPECT_EQ(1U, size);
  ASSERT_EQ(1U, r.size());
  EXPECT_EQ(r[0], STR("a"));
  r.clear();

  size = Tokenize(STR("\ta\t\nb\tcc"), STR("\n"), &r);
  EXPECT_EQ(2U, size);
  ASSERT_EQ(2U, r.size());
  EXPECT_EQ(r[0], STR("\ta\t"));
  EXPECT_EQ(r[1], STR("b\tcc"));
  r.clear();
}

TEST(StringUtilTest, TokenizeStdString)
{
  TokenizeTest<std::string>();
}

// Test for JoinString
TEST(StringUtilTest, JoinString)
{
  std::vector<std::string> in;
  EXPECT_EQ("", JoinString(in, ','));

  in.push_back("a");
  EXPECT_EQ("a", JoinString(in, ','));

  in.push_back("b");
  in.push_back("c");
  EXPECT_EQ("a,b,c", JoinString(in, ','));

  in.push_back("");
  EXPECT_EQ("a,b,c,", JoinString(in, ','));
  in.push_back(" ");
  EXPECT_EQ("a|b|c|| ", JoinString(in, '|'));
}

TEST(StringUtilTest, MatchPatternTest)
{
  EXPECT_TRUE(MatchPattern("www.google.com", "*.com"));
  EXPECT_TRUE(MatchPattern("www.google.com", "*"));
  
  EXPECT_FALSE(MatchPattern("www.google.com", "www*.g*.org"));
  
  EXPECT_TRUE(MatchPattern("Hello", "H?l?o"));
  
  EXPECT_FALSE(MatchPattern("www.google.com", "http://*)"));
  EXPECT_FALSE(MatchPattern("www.msn.com", "*.COM"));
  
  EXPECT_TRUE(MatchPattern("Hello*1234", "He??o\\*1*"));
  
  EXPECT_FALSE(MatchPattern("", "*.*"));
  
  EXPECT_TRUE(MatchPattern("", "*"));
  EXPECT_TRUE(MatchPattern("", "?"));
  EXPECT_TRUE(MatchPattern("", ""));
  
  EXPECT_FALSE(MatchPattern("Hello", ""));
  
  EXPECT_TRUE(MatchPattern("Hello*", "Hello*"));
  
  // Stop after a certain recursion depth.
  EXPECT_FALSE(MatchPattern("123456789012345678", "?????????????????*"));

  // Test UTF8 matching.
  EXPECT_TRUE(MatchPattern("heart: \xe2\x99\xa0", "*\xe2\x99\xa0"));
  EXPECT_TRUE(MatchPattern("heart: \xe2\x99\xa0.", "heart: ?."));
  EXPECT_TRUE(MatchPattern("hearts: \xe2\x99\xa0\xe2\x99\xa0", "*"));
  // Invalid sequences should be handled as a single invalid character.
  EXPECT_TRUE(MatchPattern("invalid: \xef\xbf\xbe", "invalid: ?"));
  
  // If the pattern has invalid characters, it shouldn't match anything.
  EXPECT_FALSE(MatchPattern("\xf4\x90\x80\x80", "\xf4\x90\x80\x80"));

  EXPECT_TRUE(MatchPattern("('aa bb', 'cc dd')", "(*,*)"));
  EXPECT_TRUE(MatchPattern("('aa bb', 'cc dd')", "(*,*)"));
  EXPECT_TRUE(MatchPattern("('aa bb')", "('*')"));
}


TEST(StringUtilTest, PrintVector)
{
  std::vector<int> array = {1, 2, 3, 4, 5};
  PrintVector(array);

  std::vector<std::vector<int> > array2 = {
    {1, 2, 3, 4},
    {5, 6, 7, 8, 9},
    {10, 11, 12, 13, 14,15}
  };
  PrintVector(array2);
}

TEST(StringUtilTest, PrintArray)
{
  int array[5] = {1, 2, 3, 4, 5};
  PrintArray(array, 5);
}

// ------------------------------------------------------------
// --------------------- Select End -------------------------
// ------------------------------------------------------------

} // namespace util
