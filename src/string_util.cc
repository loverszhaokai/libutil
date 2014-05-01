/******************************************************************************
 
  libutil
  
  Author: zhaokai
  
  Email: loverszhao@gmail.com

  Reference: chromium

  Description:

  Version: 1.0

******************************************************************************/

#include "util/string_util.h"

#include <algorithm>
#include <cstring>

#include "util/icu_utf.h"
#include "util/string_number_conversions.h"
#include "util/string_split.h"

namespace util
{

#define WHITESPACE_UNICODE                              \
  0x0009, /* <control-0009> to <control-000D> */        \
    0x000A,                                             \
    0x000B,                                             \
    0x000C,                                             \
    0x000D,                                             \
    0x0020, /* Space */                                 \
    0x0085, /* <control-0085> */                        \
    0x00A0, /* No-Break Space */                        \
    0x1680, /* Ogham Space Mark */                      \
    0x180E, /* Mongolian Vowel Separator */             \
    0x2000, /* En Quad to Hair Space */                 \
    0x2001,                                             \
    0x2002,                                             \
    0x2003,                                             \
    0x2004,                                             \
    0x2005,                                             \
    0x2006,                                             \
    0x2007,                                             \
    0x2008,                                             \
    0x2009,                                             \
    0x200A,                                             \
    0x200C, /* Zero Width Non-Joiner */                 \
    0x2028, /* Line Separator */                        \
    0x2029, /* Paragraph Separator */                   \
    0x202F, /* Narrow No-Break Space */                 \
    0x205F, /* Medium Mathematical Space */             \
    0x3000, /* Ideographic Space */                     \
    0

const wchar_t kWhitespaceWide[] = {
  WHITESPACE_UNICODE
};

const char kWhitespaceASCII[] = {
  0x09,    // <control-0009> to <control-000D>
  0x0A,
  0x0B,
  0x0C,
  0x0D,
  0x20,    // Space
  0
};

// ------------------------------------------------------------
// --------------------- Update Begin -------------------------
// ------------------------------------------------------------

template<typename STR>
bool ReplaceCharsT(const STR& input,
                   const typename STR::value_type replace_chars[],
                   const STR& replace_with,
                   STR* output)
{
  bool removed = false;
  size_t replace_length = replace_with.length();

  *output = input;

  size_t found = output->find_first_of(replace_chars);
  while (found != STR::npos)
  {
    removed = true;
    output->replace(found, 1, replace_with);
    found = output->find_first_of(replace_chars, found + replace_length);
  }

  return removed;
}

bool
ReplaceChars(const std::string& input,
             const char replace_chars[],
             const std::string& replace_with,
             std::string* output)
{
  return ReplaceCharsT(input, replace_chars, replace_with, output);
}

bool
ReplaceChars(const std::string &input,
             const std::string &replace,
             const std::string &replace_with,
             std::string* output)
{
  return ReplaceChars(input, replace.c_str(), replace_with, output);
}

bool
RemoveChars(const std::string& input,
            const char remove_chars[],
            std::string* output)
{
  return ReplaceChars(input, remove_chars, std::string(), output);
}

bool
RemoveChars(const std::string& input,
            const std::string &remove,
            std::string* output)
{
  return ReplaceChars(input, remove, std::string(), output);
}

template<typename STR>
TrimPositions TrimStringT(const STR& input,
                          const typename STR::value_type trim_chars[],
                          TrimPositions positions,
                          STR* output)
{
  // Find the edges of leading/trailing whitespace as desired.
  const typename STR::size_type last_char = input.length() - 1;
  const typename STR::size_type first_good_char = (positions & TRIM_LEADING) ?
      input.find_first_not_of(trim_chars) : 0;
  const typename STR::size_type last_good_char = (positions & TRIM_TRAILING) ?
      input.find_last_not_of(trim_chars) : last_char;

  // When the string was all whitespace, report that we stripped off whitespace
  // from whichever position the caller was interested in.  For empty input, we
  // stripped no whitespace, but we still need to clear |output|.
  if (input.empty() ||
      (first_good_char == STR::npos) || (last_good_char == STR::npos))
  {
    bool input_was_empty = input.empty();  // in case output == &input
    output->clear();
    return input_was_empty ? TRIM_NONE : positions;
  }

  // Trim the whitespace.
  *output =
      input.substr(first_good_char, last_good_char - first_good_char + 1);

  // Return where we trimmed from.
  return static_cast<TrimPositions>(
      ((first_good_char == 0) ? TRIM_NONE : TRIM_LEADING) |
      ((last_good_char == last_char) ? TRIM_NONE : TRIM_TRAILING));
}

bool
TrimString(const std::string& input,
           const char trim_chars[],
           std::string* output)
{
  return TrimStringT(input, trim_chars, TRIM_ALL, output) != TRIM_NONE;
}

TrimPositions
TrimWhitespaceASCII(const std::string& input,
                    TrimPositions positions,
                    std::string* output)
{
  return TrimStringT(input, kWhitespaceASCII, positions, output);
}

// This function is only for backward-compatibility.
// To be removed when all callers are updated.
TrimPositions
TrimWhitespace(const std::string& input,
               TrimPositions positions,
               std::string* output)
{
  return TrimWhitespaceASCII(input, positions, output);
}


template<typename STR>
STR CollapseWhitespaceT(const STR& text,
                        bool trim_sequences_with_line_breaks)
{
  STR result;
  result.resize(text.size());

  // Set flags to pretend we're already in a trimmed whitespace sequence, so we
  // will trim any leading whitespace.
  bool in_whitespace = true;
  bool already_trimmed = true;

  int chars_written = 0;
  for (typename STR::const_iterator i(text.begin()); i != text.end(); ++i)
  {
    if (IsWhitespace(*i))
    {
      if (!in_whitespace)
      {
        // Reduce all whitespace sequences to a single space.
        in_whitespace = true;
        result[chars_written++] = L' ';
      }
      if (trim_sequences_with_line_breaks && !already_trimmed &&
          ((*i == '\n') || (*i == '\r')))
      {
        // Whitespace sequences containing CR or LF are eliminated entirely.
        already_trimmed = true;
        --chars_written;
      }
    }
    else
    {
      // Non-whitespace chracters are copied straight across.
      in_whitespace = false;
      already_trimmed = false;
      result[chars_written++] = *i;
    }
  }

  if (in_whitespace && !already_trimmed)
  {
    // Any trailing whitespace is eliminated.
    --chars_written;
  }

  result.resize(chars_written);
  return result;
}

std::string
CollapseWhitespaceASCII(const std::string& text,
                        bool trim_sequences_with_line_breaks)
{
  return CollapseWhitespaceT(text, trim_sequences_with_line_breaks);
}

bool
FormatHexString(std::string &_input)
{
  std::string temp_input = _input;
  ReplaceChars(temp_input, " ", "", &temp_input);

  if (0 != temp_input.length() % 2)
  {
    // e.g. @_input = "i"
    return false;
  }
  else
  {
    const std::string::const_iterator it =
        std::find_if_not(temp_input.begin(), temp_input.end(), IsHexDigit<char>);

    if (temp_input.end() != it)
    {
      // e.g. @_input="i love dog"
      return false;
    }
    else
    {
      std::string::iterator it = temp_input.begin() + 2;
      while (it < temp_input.end())
      {
        it = temp_input.insert(it, ' ');
        it += 3;
      }
      _input = temp_input;
      return true;
    }
  }
}

void
Reverse(std::string _input, std::string *_output, const size_t _step)
{
  if (_step < _input.length())
  {
    const int last_pos = _input.length() - _input.length()%_step;
    const std::string last = _input.substr(last_pos);
    _input = _input.substr(0, last_pos);
    for (int i = 0; i < _input.length() / 2; i += _step)
    {
      const std::string temp = _input.substr(i, _step);
      _input.replace(i , _step,
                     _input.substr(_input.length() - (i + _step), _step));
      _input.replace(_input.length() - (i + _step), _step,
                     temp);
    }
    _input = last + _input;
  }
  *_output = _input;
}

bool
IpToHexString(const std::string &_ip, std::string *_hex_string)
{
  if (IsIp(_ip))
  {
    // @_ip = XXX.XXX.XXX.XXX
    std::vector<std::string> ip_vec;
    SplitStringUsingSubstr(_ip, ".", &ip_vec);

    *_hex_string = "";
    for (int i = 0; i < 4; ++i)
    {
      unsigned int temp_uint = 0;
      if (!StringToUint(ip_vec[i], &temp_uint))
        return false;
      std::string temp_str = "";
      UintToHexString(temp_uint, &temp_str);
      *_hex_string += temp_str;
    }
    FormatHexString(*_hex_string);
    
    return true;
  }
  return false;
}

bool
HexStringToIp(std::string _hex_string, std::string *_ip)
{
  if (IsIp(_hex_string))
  {
    ReplaceChars(_hex_string, " ","", &_hex_string);

    *_ip = "";
    for (int i = 0; i < _hex_string.length() - 1; i += 2)
    {
      unsigned int temp_uint = 0;
      if (!HexStringToInt(_hex_string.substr(i, 2), (int*)&temp_uint))
        return false;
      *_ip += ToString(temp_uint) + ".";
    }
    *_ip = (*_ip).substr(0, (*_ip).length() - 1);
    
    return true;
  }

  return false;
}


  
// ------------------------------------------------------------
// --------------------- Update End -------------------------
// ------------------------------------------------------------

  
// ------------------------------------------------------------
// --------------------- Select Begin -------------------------
// ------------------------------------------------------------


bool
ContainsOnlyWhitespaceASCII(const std::string& str)
{
  for (std::string::const_iterator i(str.begin()); i != str.end(); ++i)
  {
    if (!IsAsciiWhitespace(*i))
      return false;
  }
  return true;
}

template<typename STR>
static bool ContainsOnlyCharsT(const STR& input, const STR& characters)
{
  for (typename STR::const_iterator iter = input.begin();
       iter != input.end(); ++iter)
  {
    if (characters.find(*iter) == STR::npos)
      return false;
  }
  return true;
}

bool
ContainsOnlyChars(const std::string& input,
                  const std::string& characters)
{
  return ContainsOnlyCharsT(input, characters);
}

template<typename Iter>
static inline bool DoLowerCaseEqualsASCII(Iter a_begin,
                                          Iter a_end,
                                          const char* b)
{
  for (Iter it = a_begin; it != a_end; ++it, ++b)
  {
    if (!*b || ToLowerASCII(*it) != *b)
      return false;
  }
  return *b == 0;
}

// Front-ends for LowerCaseEqualsASCII.
bool
LowerCaseEqualsASCII(const std::string& a, const char* b)
{
  return DoLowerCaseEqualsASCII(a.begin(), a.end(), b);
}

bool
LowerCaseEqualsASCII(std::string::const_iterator a_begin,
                     std::string::const_iterator a_end,
                     const char* b)
{
  return DoLowerCaseEqualsASCII(a_begin, a_end, b);
}

// TODO(port): Resolve wchar_t/iterator issues that require OS_ANDROID here.
bool
LowerCaseEqualsASCII(const char* a_begin,
                     const char* a_end,
                     const char* b)
{
  return DoLowerCaseEqualsASCII(a_begin, a_end, b);
}

bool
StartsWithASCII(const std::string& str,
                const std::string& search,
                bool case_sensitive)
{
  if (case_sensitive)
    return str.compare(0, search.length(), search) == 0;
  else
    return strncasecmp(str.c_str(), search.c_str(), search.length()) == 0;
}

template <typename STR>
bool EndsWithT(const STR& str, const STR& search, bool case_sensitive)
{
  typename STR::size_type str_length = str.length();
  typename STR::size_type search_length = search.length();
  if (search_length > str_length)
    return false;
  if (case_sensitive)
  {
    return str.compare(str_length - search_length, search_length, search) == 0;
  }
  else
  {
    return std::equal(search.begin(), search.end(),
                      str.begin() + (str_length - search_length),
                      CaseInsensitiveCompare<typename STR::value_type>());
  }
}

bool
EndsWith(const std::string& str,
         const std::string& search,
         bool case_sensitive)
{
  return EndsWithT(str, search, case_sensitive);
}

template<typename STR>
static size_t TokenizeT(const STR& str,
                        const STR& delimiters,
                        std::vector<STR>* tokens)
{
  tokens->clear();

  typename STR::size_type start = str.find_first_not_of(delimiters);
  while (start != STR::npos)
  {
    typename STR::size_type end = str.find_first_of(delimiters, start + 1);
    if (end == STR::npos)
    {
      tokens->push_back(str.substr(start));
      break;
    }
    else
    {
      tokens->push_back(str.substr(start, end - start));
      start = str.find_first_not_of(delimiters, end + 1);
    }
  }

  return tokens->size();
}

size_t
Tokenize(const std::string& str,
         const std::string& delimiters,
         std::vector<std::string>* tokens)
{
  return TokenizeT(str, delimiters, tokens);
}


template<typename STR>
static STR JoinStringT(const std::vector<STR>& parts,
                       typename STR::value_type sep)
{
  if (parts.empty())
    return STR();

  STR result(parts[0]);
  typename std::vector<STR>::const_iterator iter = parts.begin();
  ++iter;

  for (; iter != parts.end(); ++iter)
  {
    result += sep;
    result += *iter;
  }

  return result;
}

std::string
JoinString(const std::vector<std::string>& parts, char sep)
{
  return JoinStringT(parts, sep);
}



static bool IsWildcard(base_icu::UChar32 character)
{
  return character == '*' || character == '?';
}

// Move the strings pointers to the point where they start to differ.
template <typename CHAR, typename NEXT>
static void EatSameChars(const CHAR** pattern, const CHAR* pattern_end,
                         const CHAR** string, const CHAR* string_end,
                         NEXT next)
{
  const CHAR* escape = NULL;
  while (*pattern != pattern_end && *string != string_end)
  {
    if (!escape && IsWildcard(**pattern))
    {
      // We don't want to match wildcard here, except if it's escaped.
      return;
    }

    // Check if the escapement char is found. If so, skip it and move to the
    // next character.
    if (!escape && **pattern == '\\')
    {
      escape = *pattern;
      next(pattern, pattern_end);
      continue;
    }

    // Check if the chars match, if so, increment the ptrs.
    const CHAR* pattern_next = *pattern;
    const CHAR* string_next = *string;
    base_icu::UChar32 pattern_char = next(&pattern_next, pattern_end);
    if (pattern_char == next(&string_next, string_end) &&
        pattern_char != (base_icu::UChar32) CBU_SENTINEL)
    {
      *pattern = pattern_next;
      *string = string_next;
    }
    else
    {
      // Uh ho, it did not match, we are done. If the last char was an
      // escapement, that means that it was an error to advance the ptr here,
      // let's put it back where it was. This also mean that the MatchPattern
      // function will return false because if we can't match an escape char
      // here, then no one will.
      if (escape)
      {
        *pattern = escape;
      }
      return;
    }

    escape = NULL;
  }
}

template <typename CHAR, typename NEXT>
static void EatWildcard(const CHAR** pattern, const CHAR* end, NEXT next)
{
  while (*pattern != end)
  {
    if (!IsWildcard(**pattern))
      return;
    next(pattern, end);
  }
}

template <typename CHAR, typename NEXT>
static bool MatchPatternT(const CHAR* eval, const CHAR* eval_end,
                          const CHAR* pattern, const CHAR* pattern_end,
                          int depth,
                          NEXT next)
{
  const int kMaxDepth = 16;
  if (depth > kMaxDepth)
    return false;

  // Eat all the matching chars.
  EatSameChars(&pattern, pattern_end, &eval, eval_end, next);

  // If the string is empty, then the pattern must be empty too, or contains
  // only wildcards.
  if (eval == eval_end)
  {
    EatWildcard(&pattern, pattern_end, next);
    return pattern == pattern_end;
  }

  // Pattern is empty but not string, this is not a match.
  if (pattern == pattern_end)
    return false;

  // If this is a question mark, then we need to compare the rest with
  // the current string or the string with one character eaten.
  const CHAR* next_pattern = pattern;
  next(&next_pattern, pattern_end);
  if (pattern[0] == '?')
  {
    if (MatchPatternT(eval, eval_end, next_pattern, pattern_end,
                      depth + 1, next))
      return true;
    const CHAR* next_eval = eval;
    next(&next_eval, eval_end);
    if (MatchPatternT(next_eval, eval_end, next_pattern, pattern_end,
                      depth + 1, next))
      return true;
  }

  // This is a *, try to match all the possible substrings with the remainder
  // of the pattern.
  if (pattern[0] == '*')
  {
    // Collapse duplicate wild cards (********** into *) so that the
    // method does not recurse unnecessarily. http://crbug.com/52839
    EatWildcard(&next_pattern, pattern_end, next);

    while (eval != eval_end)
    {
      if (MatchPatternT(eval, eval_end, next_pattern, pattern_end,
                        depth + 1, next))
        return true;
      eval++;
    }

    // We reached the end of the string, let see if the pattern contains only
    // wildcards.
    if (eval == eval_end)
    {
      EatWildcard(&pattern, pattern_end, next);
      if (pattern != pattern_end)
        return false;
      return true;
    }
  }

  return false;
}

struct NextCharUTF8
{
  base_icu::UChar32 operator()(const char** p, const char* end)
  {
    base_icu::UChar32 c;
    int offset = 0;
    CBU8_NEXT(*p, offset, end - *p, c);
    *p += offset;
    return c;
  }
};

bool
MatchPattern(const std::string& eval,
             const std::string& pattern)
{
  return MatchPatternT(eval.data(), eval.data() + eval.size(),
                       pattern.data(), pattern.data() + pattern.size(),
                       0, NextCharUTF8());
}

bool
IsIp(const std::string &_ip)
{
  return true;
}

bool
IsMac(const std::string &_mac)
{
  return true;
}

bool
IsPort(const std::string &_port)
{
  return true;
}

// ------------------------------------------------------------
// --------------------- Select End -------------------------
// ------------------------------------------------------------
}; // namespace util
