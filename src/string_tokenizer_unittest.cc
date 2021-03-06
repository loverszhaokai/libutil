/******************************************************************************
 
  libutil
  
  Author: zhaokai
  
  Email: loverszhao@gmail.com

  Reference: chromium

  Description:

  Version: 1.0

******************************************************************************/

#include "util/string_tokenizer.h"

#include "third_party/gtest/include/gtest/gtest.h"

using std::string;

namespace util
{

namespace
{
  class StringTokenizerTest : public testing::Test
  {};
}

TEST(StringTokenizerTest, Simple)
{
  string input = "this is a test";
  StringTokenizer t(input, " ");

  EXPECT_TRUE(t.GetNext());
  EXPECT_EQ(string("this"), t.token());

  EXPECT_TRUE(t.GetNext());
  EXPECT_EQ(string("is"), t.token());

  EXPECT_TRUE(t.GetNext());
  EXPECT_EQ(string("a"), t.token());

  EXPECT_TRUE(t.GetNext());
  EXPECT_EQ(string("test"), t.token());

  EXPECT_FALSE(t.GetNext());
}

TEST(StringTokenizerTest, Simple2)
{
  string input = "this is a test";
  StringTokenizer t(input, "is");

  EXPECT_TRUE(t.GetNext());
  EXPECT_EQ(string("th"), t.token());

  EXPECT_TRUE(t.GetNext());
  EXPECT_EQ(string(" "), t.token());

  EXPECT_TRUE(t.GetNext());
  EXPECT_EQ(string(" a te"), t.token());

  EXPECT_TRUE(t.GetNext());
  EXPECT_EQ(string("t"), t.token());

  EXPECT_FALSE(t.GetNext());
}

TEST(StringTokenizerTest, Reset)
{
  string input = "this is a test";
  StringTokenizer t(input, " ");

  for (int i = 0; i < 2; ++i)
  {
    EXPECT_TRUE(t.GetNext());
    EXPECT_EQ(string("this"), t.token());

    EXPECT_TRUE(t.GetNext());
    EXPECT_EQ(string("is"), t.token());

    EXPECT_TRUE(t.GetNext());
    EXPECT_EQ(string("a"), t.token());

    EXPECT_TRUE(t.GetNext());
    EXPECT_EQ(string("test"), t.token());

    EXPECT_FALSE(t.GetNext());
    t.Reset();
  }
}

TEST(StringTokenizerTest, RetDelims)
{
  string input = "this is a test";
  StringTokenizer t(input, " ");
  t.set_options(StringTokenizer::RETURN_DELIMS);

  EXPECT_TRUE(t.GetNext());
  EXPECT_EQ(string("this"), t.token());

  EXPECT_TRUE(t.GetNext());
  EXPECT_EQ(string(" "), t.token());

  EXPECT_TRUE(t.GetNext());
  EXPECT_EQ(string("is"), t.token());

  EXPECT_TRUE(t.GetNext());
  EXPECT_EQ(string(" "), t.token());

  EXPECT_TRUE(t.GetNext());
  EXPECT_EQ(string("a"), t.token());

  EXPECT_TRUE(t.GetNext());
  EXPECT_EQ(string(" "), t.token());

  EXPECT_TRUE(t.GetNext());
  EXPECT_EQ(string("test"), t.token());

  EXPECT_FALSE(t.GetNext());
}

TEST(StringTokenizerTest, ManyDelims)
{
  string input = "this: is, a-test";
  StringTokenizer t(input, ": ,-");

  EXPECT_TRUE(t.GetNext());
  EXPECT_EQ(string("this"), t.token());

  EXPECT_TRUE(t.GetNext());
  EXPECT_EQ(string("is"), t.token());

  EXPECT_TRUE(t.GetNext());
  EXPECT_EQ(string("a"), t.token());

  EXPECT_TRUE(t.GetNext());
  EXPECT_EQ(string("test"), t.token());

  EXPECT_FALSE(t.GetNext());
}

TEST(StringTokenizerTest, ParseHeader)
{
  string input = "Content-Type: text/html ; charset=UTF-8";
  StringTokenizer t(input, ": ;=");
  t.set_options(StringTokenizer::RETURN_DELIMS);

  EXPECT_TRUE(t.GetNext());
  EXPECT_FALSE(t.token_is_delim());
  EXPECT_EQ(string("Content-Type"), t.token());

  EXPECT_TRUE(t.GetNext());
  EXPECT_TRUE(t.token_is_delim());
  EXPECT_EQ(string(":"), t.token());

  EXPECT_TRUE(t.GetNext());
  EXPECT_TRUE(t.token_is_delim());
  EXPECT_EQ(string(" "), t.token());

  EXPECT_TRUE(t.GetNext());
  EXPECT_FALSE(t.token_is_delim());
  EXPECT_EQ(string("text/html"), t.token());

  EXPECT_TRUE(t.GetNext());
  EXPECT_TRUE(t.token_is_delim());
  EXPECT_EQ(string(" "), t.token());

  EXPECT_TRUE(t.GetNext());
  EXPECT_TRUE(t.token_is_delim());
  EXPECT_EQ(string(";"), t.token());

  EXPECT_TRUE(t.GetNext());
  EXPECT_TRUE(t.token_is_delim());
  EXPECT_EQ(string(" "), t.token());

  EXPECT_TRUE(t.GetNext());
  EXPECT_FALSE(t.token_is_delim());
  EXPECT_EQ(string("charset"), t.token());

  EXPECT_TRUE(t.GetNext());
  EXPECT_TRUE(t.token_is_delim());
  EXPECT_EQ(string("="), t.token());

  EXPECT_TRUE(t.GetNext());
  EXPECT_FALSE(t.token_is_delim());
  EXPECT_EQ(string("UTF-8"), t.token());

  EXPECT_FALSE(t.GetNext());
  EXPECT_FALSE(t.token_is_delim());
}

TEST(StringTokenizerTest, ParseQuotedString)
{
  string input = "foo bar 'hello world' baz";
  StringTokenizer t(input, " ");
  t.set_quote_chars("'");

  EXPECT_TRUE(t.GetNext());
  EXPECT_EQ(string("foo"), t.token());

  EXPECT_TRUE(t.GetNext());
  EXPECT_EQ(string("bar"), t.token());

  EXPECT_TRUE(t.GetNext());
  EXPECT_EQ(string("'hello world'"), t.token());

  EXPECT_TRUE(t.GetNext());
  EXPECT_EQ(string("baz"), t.token());

  EXPECT_FALSE(t.GetNext());
}

TEST(StringTokenizerTest, ParseQuotedString_Malformed)
{
  string input = "bar 'hello wo";
  StringTokenizer t(input, " ");
  t.set_quote_chars("'");

  EXPECT_TRUE(t.GetNext());
  EXPECT_EQ(string("bar"), t.token());

  EXPECT_TRUE(t.GetNext());
  EXPECT_EQ(string("'hello wo"), t.token());

  EXPECT_FALSE(t.GetNext());
}

TEST(StringTokenizerTest, ParseQuotedString_Multiple)
{
  string input = "bar 'hel\"lo\" wo' baz\"";
  StringTokenizer t(input, " ");
  t.set_quote_chars("'\"");

  EXPECT_TRUE(t.GetNext());
  EXPECT_EQ(string("bar"), t.token());

  EXPECT_TRUE(t.GetNext());
  EXPECT_EQ(string("'hel\"lo\" wo'"), t.token());

  EXPECT_TRUE(t.GetNext());
  EXPECT_EQ(string("baz\""), t.token());

  EXPECT_FALSE(t.GetNext());
}

TEST(StringTokenizerTest, ParseQuotedString_EscapedQuotes)
{
  string input = "foo 'don\\'t do that'";
  StringTokenizer t(input, " ");
  t.set_quote_chars("'");

  EXPECT_TRUE(t.GetNext());
  EXPECT_EQ(string("foo"), t.token());

  EXPECT_TRUE(t.GetNext());
  EXPECT_EQ(string("'don\\'t do that'"), t.token());

  EXPECT_FALSE(t.GetNext());
}

TEST(StringTokenizerTest, ParseQuotedString_EscapedQuotes2)
{
  string input = "foo='a, b', bar";
  StringTokenizer t(input, ", ");
  t.set_quote_chars("'");

  EXPECT_TRUE(t.GetNext());
  EXPECT_EQ(string("foo='a, b'"), t.token());

  EXPECT_TRUE(t.GetNext());
  EXPECT_EQ(string("bar"), t.token());

  EXPECT_FALSE(t.GetNext());
}

}; // namespace util
