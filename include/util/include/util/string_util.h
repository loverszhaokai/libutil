/******************************************************************************
 
  libutil
  
  Author: zhaokai
  
  Email: loverszhao@gmail.com

  Reference: chromium

  Description:

  Version: 1.0

******************************************************************************/

#ifndef UTIL_STRING_UTIL_H_
#define UTIL_STRING_UTIL_H_

#include <iostream>
#include <string>
#include <vector>

namespace util
{
  extern const wchar_t kWhitespaceWide[];

  // ------------------------------------------------------------
  // --------------------- Update Begin -------------------------
  // ------------------------------------------------------------

    
  // ASCII-specific tolower.  The standard library's tolower is locale sensitive,
  // so we don't want to use it here.
  template <class Char>
      inline Char ToLowerASCII(Char c)
  {
    return (c >= 'A' && c <= 'Z') ? (c + ('a' - 'A')) : c;
  }

  // ASCII-specific toupper.  The standard library's toupper is locale sensitive,
  // so we don't want to use it here.
  template <class Char>
      inline Char ToUpperASCII(Char c)
  {
    return (c >= 'a' && c <= 'z') ? (c + ('A' - 'a')) : c;
  }

    
  // Replaces characters in |replace_chars| from anywhere in |input| with
  // |replace_with|.  Each character in |replace_chars| will be replaced with
  // the |replace_with| string.  Returns true if any characters were replaced.
  // |replace_chars| must be null-terminated.
  // NOTE: Safe to use the same variable for both |input| and |output|.
  bool ReplaceChars(const std::string& input,
                    const char replace_chars[],
                    const std::string& replace_with,
                    std::string* output);

  bool ReplaceChars(const std::string& input,
                    const std::string &replace,
                    const std::string& replace_with,
                    std::string* output);

  // Removes characters in |remove_chars| from anywhere in |input|.  Returns true
  // if any characters were removed.  |remove_chars| must be null-terminated.
  // NOTE: Safe to use the same variable for both |input| and |output|.
  bool RemoveChars(const std::string& input,
                   const char remove_chars[],
                   std::string* output);

  bool RemoveChars(const std::string& input,
                   const std::string &remove,
                   std::string* output);

  // Removes characters in |trim_chars| from the beginning and end of |input|.
  // |trim_chars| must be null-terminated.
  // NOTE: Safe to use the same variable for both |input| and |output|.
  bool TrimString(const std::string& input,
                  const char trim_chars[],
                  std::string* output);

  // Trims any whitespace from either end of the input string.  Returns where
  // whitespace was found.
  // The non-wide version has two functions:
  // * TrimWhitespaceASCII()
  //   This function is for ASCII strings and only looks for ASCII whitespace;
  // Please choose the best one according to your usage.
  // NOTE: Safe to use the same variable for both input and output.
  enum TrimPositions
  {
    TRIM_NONE     = 0,
    TRIM_LEADING  = 1 << 0,
    TRIM_TRAILING = 1 << 1,
    TRIM_ALL      = TRIM_LEADING | TRIM_TRAILING,
  };

  TrimPositions TrimWhitespaceASCII(const std::string& input,
                                    TrimPositions positions,
                                    std::string* output);

  // Deprecated. This function is only for backward compatibility and calls
  // TrimWhitespaceASCII().
  TrimPositions TrimWhitespace(const std::string& input,
                               TrimPositions positions,
                               std::string* output);

  
  // Searches  for CR or LF characters.  Removes all contiguous whitespace
  // strings that contain them.  This is useful when trying to deal with text
  // copied from terminals.
  // Returns |text|, with the following three transformations:
  // (1) Leading and trailing whitespace is trimmed.
  // (2) If |trim_sequences_with_line_breaks| is true, any other whitespace
  //     sequences containing a CR or LF are trimmed.
  // (3) All other whitespace sequences are converted to single spaces.
  std::string CollapseWhitespaceASCII(const std::string& text,
                                      bool trim_sequences_with_line_breaks);

  // Converts the elements of the given string.  This version uses a pointer to
  // clearly differentiate it from the non-pointer variant.
  template <class str>
      inline void StringToLowerASCII(str* s)
  {
    for (typename str::iterator i = s->begin(); i != s->end(); ++i)
      *i = ToLowerASCII(*i);
  }

  template <class str>
      inline str StringToLowerASCII(const str& s)
  {
    // for std::string and std::wstring
    str output(s);
    StringToLowerASCII(&output);
    return output;
  }

  // Converts the elements of the given string.  This version uses a pointer to
  // clearly differentiate it from the non-pointer variant.
  template <class str>
      inline void StringToUpperASCII(str* s)
  {
    for (typename str::iterator i = s->begin(); i != s->end(); ++i)
      *i = ToUpperASCII(*i);
  }

  template <class str>
      inline str StringToUpperASCII(const str& s)
  {
    // for std::string and std::wstring
    str output(s);
    StringToUpperASCII(&output);
    return output;
  }

  //
  // Remove all the Whitespace of @_input firstly
  // Return false when
  //   if the length of @_input is odd return false
  //   or find the char is not between '0'~'9' or 'a'~'z' or 'A'~'Z' return false
  // Otherwise, return true
  //   format @_input, such as "XX_XX_XX"
  // 
  bool FormatHexString(std::string &_input);

  //
  // e.g. @_input = "11223344" @_step=2
  //      after Reverse() @_output="44332211"
  // 
  void Reverse(std::string _input, std::string *_output, const size_t _step);

  // 
  // @_ip     = XXX.XXX.XXX.XXX
  // @_output = XX XX XX XX
  //
  // return false in case of @_ip is illegal
  // otherwise return true
  // 
  bool IpToHexString(const std::string &_ip, std::string *_hex_string);

  // 
  // @_hex_string = XX XX XX XX
  // @_ip         = XXX.XXX.XXX.XXX
  // 
  bool HexStringToIp(std::string _hex_string, std::string *_ip);

  // ------------------------------------------------------------
  // --------------------- Update End -------------------------
  // ------------------------------------------------------------

  
  // ------------------------------------------------------------
  // --------------------- Select Begin -------------------------
  // ------------------------------------------------------------

  
  // Determines the type of ASCII character, independent of locale (the C
  // library versions will change based on locale).
  template <typename Char>
      inline bool IsAsciiWhitespace(Char c)
  {
    return c == ' ' || c == '\r' || c == '\n' || c == '\t';
  }

  template <typename Char>
      inline bool IsAsciiAlpha(Char c)
  {
    return ((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z'));
  }
  
  template <typename Char>
      inline bool IsAsciiDigit(Char c)
  {
    return c >= '0' && c <= '9';
  }

  template <typename Char>
      inline bool IsHexDigit(Char c)
  {
    return (c >= '0' && c <= '9') ||
        (c >= 'A' && c <= 'F') ||
        (c >= 'a' && c <= 'f');
  }

  template <typename Char>
      inline Char HexDigitToInt(Char c)
  {
    if (c >= '0' && c <= '9')
      return c - '0';
    if (c >= 'A' && c <= 'F')
      return c - 'A' + 10;
    if (c >= 'a' && c <= 'f')
      return c - 'a' + 10;
    return 0;
  }

  // Returns true if it's a whitespace character.
  inline bool IsWhitespace(wchar_t c)
  {
    return wcschr(kWhitespaceWide, c) != NULL;
  }

  // Function objects to aid in comparing/searching strings.
  template<typename Char>
      struct CaseInsensitiveCompare
      {
       public:
        bool operator()(Char x, Char y) const
        {
          // TODO(darin): Do we really want to do locale sensitive comparisons here?
          // See http://crbug.com/24917
          return tolower(x) == tolower(y);
        }
      };

  // Returns true if the passed string is empty or contains only white-space
  // characters.
  bool ContainsOnlyWhitespaceASCII(const std::string& str);

  // Returns true if |input| is empty or contains only characters found in
  // |characters|.
  bool ContainsOnlyChars(const std::string& input,
                         const std::string& characters);

  // Compare the lower-case form of the given string against the given ASCII
  // string.  This is useful for doing checking if an input string matches some
  // token, and it is optimized to avoid intermediate string copies.  This API is
  // borrowed from the equivalent APIs in Mozilla.
  bool LowerCaseEqualsASCII(const std::string& a, const char* b);

  // Same thing, but with string iterators instead.
  bool LowerCaseEqualsASCII(std::string::const_iterator a_begin,
                           std::string::const_iterator a_end,
                           const char* b);
  bool LowerCaseEqualsASCII(const char* a_begin,
                            const char* a_end,
                            const char* b);

  // Performs a case-sensitive string compare. The behavior is undefined if both
  // strings are not ASCII.
  // Returns true if str starts with search, or false otherwise.
  bool StartsWithASCII(const std::string& str,
                       const std::string& search,
                       bool case_sensitive);

  // Returns true if str ends with search, or false otherwise.
  bool EndsWith(const std::string& str,
                const std::string& search,
                bool case_sensitive);

  // Splits a string into its fields delimited by any of the characters in
  // |delimiters|.  Each field is added to the |tokens| vector.  Returns the
  // number of tokens found.
  size_t Tokenize(const std::string& str,
                  const std::string& delimiters,
                  std::vector<std::string>* tokens);

  
  // Does the opposite of SplitString().
  std::string JoinString(const std::vector<std::string>& parts, char s);


  // Returns true if the string passed in matches the pattern. The pattern
  // string can contain wildcards like * and ?
  // The backslash character (\) is an escape character for * and ?
  // We limit the patterns to having a max of 16 * or ? characters.
  // ? matches 0 or 1 character, while * matches 0 or more characters.
  bool MatchPattern(const std::string& string,
                    const std::string& pattern);

  template<typename T>
      void PrintVector(const std::vector<T> &_vec)
  {
    typename std::vector<T>::const_iterator it = _vec.begin();
    while (it != _vec.end())
    {
      std::cout << "\t" << *it;
      it++;
    }
    std::cout << std::endl;
  }


  template<typename T>
      void PrintVector(const std::vector<std::vector<T> > &_vec)
  {
    typename std::vector<std::vector<T> >::const_iterator it = _vec.begin();
    while (it != _vec.end())
    {
      std::cout << "row[" << it - _vec.begin() + 1 << "]:";
      PrintVector(*it);
      it++;
    }
    std::cout << std::endl;
  }

  template<typename T>
      void PrintArray(const T *_t, const size_t _size)
  {
    for (size_t i = 0; i < _size; ++i)
    {
      std::cout << "\t" << *(_t + i);      
    }
    std::cout << std::endl;
  }

  // 
  // @_ip    = XXX.XXX.XXX.XXX
  // or @_ip = XX XX XX XX
  //
  // return false if @_ip is invalid
  // otherwise return true
  // 
  bool IsIp(const std::string &_ip);

  // 
  // @_mac    = XX:XX:XX:XX:XX:XX
  // or @_mac = XX XX XX XX XX XX
  //
  // return false if @_mac is invalid
  // otherwise return true
  // 
  bool IsMac(const std::string &_mac);

  // 
  // @_port works only for TCP
  // @_port must between 0-65535
  // 
  // return false if @_port is invalid
  // otherwise return true
  // 
  bool IsPort(const std::string &_port);

  // ------------------------------------------------------------
  // --------------------- Select End -------------------------
  // ------------------------------------------------------------
  
}; // namespace util

#endif
