/******************************************************************************
 
  libutil
  
  Author: zhaokai
  
  Email: loverszhao@gmail.com

  Reference: chromium

  Description:

  Version: 1.0

******************************************************************************/

#include "util/string_number_conversions.h"

#include <cwctype>
#include <iostream>
#include <limits>
#include <sstream>

#include "util/string_util.h"

namespace util
{

// Utility to convert a character to a digit in a given base
template<typename CHAR, int BASE, bool BASE_LTE_10>
class BaseCharToDigit
{};

// Faster specialization for bases <= 10
template<typename CHAR, int BASE>
class BaseCharToDigit<CHAR, BASE, true>
{
 public:
  static bool Convert(CHAR c, uint8* digit)
  {
    if (c >= '0' && c < '0' + BASE)
    {
      *digit = c - '0';
      return true;
    }
    return false;
  }
};

// Specialization for bases where 10 < base <= 36
template<typename CHAR, int BASE>
class BaseCharToDigit<CHAR, BASE, false>
{
 public:
  static bool Convert(CHAR c, uint8* digit)
  {
    if (c >= '0' && c <= '9')
    {
      *digit = c - '0';
    }
    else if (c >= 'a' && c < 'a' + BASE - 10)
    {
      *digit = c - 'a' + 10;
    }
    else if (c >= 'A' && c < 'A' + BASE - 10)
    {
      *digit = c - 'A' + 10;
    }
    else
    {
      return false;
    }
    return true;
  }
};

template<int BASE, typename CHAR>
bool
CharToDigit(CHAR c, uint8* digit)
{
  return BaseCharToDigit<CHAR, BASE, BASE <= 10>::Convert(c, digit);
}


// There is an IsWhitespace for wchars defined in string_util.h, but it is
// locale independent, whereas the functions we are replacing were
// locale-dependent. TBD what is desired, but for the moment let's not introduce
// a change in behaviour.
template<typename CHAR> class WhitespaceHelper
{};

template<>
class WhitespaceHelper<char>
{
 public:
  static bool Invoke(char c)
  {
    return 0 != isspace(static_cast<unsigned char>(c));
  }
};

template<>
class WhitespaceHelper<char16>
{
 public:
  static bool Invoke(char16 c)
  {
    return 0 != iswspace(c);
  }
};

template<typename CHAR>
bool
LocalIsWhitespace(CHAR c)
{
  return WhitespaceHelper<CHAR>::Invoke(c);
}

// IteratorRangeToNumberTraits should provide:
//  - a typedef for iterator_type, the iterator type used as input.
//  - a typedef for value_type, the target numeric type.
//  - static functions min, max (returning the minimum and maximum permitted
//    values)
//  - constant kBase, the base in which to interpret the input
template<typename IteratorRangeToNumberTraits>
class IteratorRangeToNumber
{
 public:
  typedef IteratorRangeToNumberTraits traits;
  typedef typename traits::iterator_type const_iterator;
  typedef typename traits::value_type value_type;

  // Generalized iterator-range-to-number conversion.
  //
  static bool Invoke(const_iterator begin,
                     const_iterator end,
                     value_type* output)
  {
    bool valid = true;

    while (begin != end && LocalIsWhitespace(*begin))
    {
      valid = false;
      ++begin;
    }

    if (begin != end && *begin == '-')
    {
      if (!Negative::Invoke(begin + 1, end, output))
      {
        valid = false;
      }
    }
    else
    {
      if (begin != end && *begin == '+')
      {
        ++begin;
      }
      if (!Positive::Invoke(begin, end, output))
      {
        valid = false;
      }
    }

    return valid;
  }

 private:
  // Sign provides:
  //  - a static function, CheckBounds, that determines whether the next digit
  //    causes an overflow/underflow
  //  - a static function, Increment, that appends the next digit appropriately
  //    according to the sign of the number being parsed.
  template<typename Sign>
  class Base
  {
   public:
    static bool Invoke(const_iterator begin, const_iterator end,
                       typename traits::value_type* output)
    {
      *output = 0;

      if (begin == end)
      {
        return false;
      }

      // Note: no performance difference was found when using template
      // specialization to remove this check in bases other than 16
      if (traits::kBase == 16 && end - begin > 2 && *begin == '0' &&
          (*(begin + 1) == 'x' || *(begin + 1) == 'X'))
      {
        begin += 2;
      }

      for (const_iterator current = begin; current != end; ++current)
      {
        uint8 new_digit = 0;

        if (!CharToDigit<traits::kBase>(*current, &new_digit))
        {
          return false;
        }

        if (current != begin)
        {
          if (!Sign::CheckBounds(output, new_digit))
          {
            return false;
          }
          *output *= traits::kBase;
        }

        Sign::Increment(new_digit, output);
      }
      return true;
    }
  };

  class Positive : public Base<Positive>
  {
   public:
    static bool CheckBounds(value_type* output, uint8 new_digit)
    {
      if (*output > static_cast<value_type>(traits::max() / traits::kBase) ||
          (*output == static_cast<value_type>(traits::max() / traits::kBase) &&
           new_digit > traits::max() % traits::kBase))
      {
        *output = traits::max();
        return false;
      }
      return true;
    }
    static void Increment(uint8 increment, value_type* output)
    {
      *output += increment;
    }
  };

  class Negative : public Base<Negative>
  {
   public:
    static bool CheckBounds(value_type* output, uint8 new_digit)
    {
      if (*output < traits::min() / traits::kBase ||
          (*output == traits::min() / traits::kBase &&
           new_digit > 0 - traits::min() % traits::kBase))
      {
        *output = traits::min();
        return false;
      }
      return true;
    }
    static void Increment(uint8 increment, value_type* output)
    {
      *output -= increment;
    }
  };
};

template<typename ITERATOR, typename VALUE, int BASE>
class BaseIteratorRangeToNumberTraits
{
 public:
  typedef ITERATOR iterator_type;
  typedef VALUE value_type;
  static value_type min()
  {
    return std::numeric_limits<value_type>::min();
  }
  static value_type max()
  {
    return std::numeric_limits<value_type>::max();
  }
  static const int kBase = BASE;
};

template<typename ITERATOR>
class BaseHexIteratorRangeToIntTraits
    : public BaseIteratorRangeToNumberTraits<ITERATOR, int, 16>
{
 public:
  // Allow parsing of 0xFFFFFFFF, which is technically an overflow
  static unsigned int max()
  {
    return std::numeric_limits<unsigned int>::max();
  }
};


template <typename VALUE, int BASE>
class StringToNumberTraits
    : public BaseIteratorRangeToNumberTraits<std::string::const_iterator,
                                             VALUE,
                                             BASE>
{};

template <typename VALUE>
bool
StringToIntImpl(const std::string &_input, VALUE *_output)
{
  return IteratorRangeToNumber<StringToNumberTraits<VALUE, 10> >::Invoke(
      _input.begin(), _input.end(), _output);
}

bool
StringToInt(const std::string &_input, int *_output)
{
  return StringToIntImpl(_input, _output);
}

bool
StringToUint(const std::string &_input, unsigned *_output)
{
  return StringToIntImpl(_input, _output);
}

bool
StringToInt64(const std::string &_input, int64 *_output)
{
  return StringToIntImpl(_input, _output);
}

bool
StringToUint64(const std::string &_input, uint64 *_output)
{
  return StringToIntImpl(_input, _output);
}

bool
StringToSizeT(const std::string &_input, size_t *_output)
{
  return StringToIntImpl(_input, _output);
}


std::string
HexEncode(const void *_bytes, size_t _size)
{
  static const char kHexChars[] = "0123456789ABCDEF";

  // Each input byte creates two output hex characters.
  std::string ret(_size * 2, '\0');

  for (size_t i = 0; i < _size; ++i)
  {
    char b = reinterpret_cast<const char*>(_bytes)[i];
    ret[(i * 2)] = kHexChars[(b >> 4) & 0xf];
    ret[(i * 2) + 1] = kHexChars[b & 0xf];
  }
  return ret;
}


typedef BaseHexIteratorRangeToIntTraits<std::string::const_iterator>
HexIteratorRangeToIntTraits;

template<typename STR>
bool HexStringToBytesT(const STR& input, std::vector<uint8>* output)
{
  size_t count = input.size();
  if (count == 0 || (count % 2) != 0)
    return false;
  for (uintptr_t i = 0; i < count / 2; ++i)
  {
    uint8 msb = 0;  // most significant 4 bits
    uint8 lsb = 0;  // least significant 4 bits
    if (!CharToDigit<16>(input[i * 2], &msb) ||
        !CharToDigit<16>(input[i * 2 + 1], &lsb))
      return false;
    output->push_back((msb << 4) | lsb);
  }
  return true;
}

bool
HexStringToInt(const std::string &_input, int *_output)
{
  return IteratorRangeToNumber<HexIteratorRangeToIntTraits>::Invoke(
      _input.begin(), _input.end(), _output);
}

bool
HexStringToBytes(const std::string &_input, std::vector<uint8> *_output)
{
  std::string inputNoWhitespace;
  ReplaceChars(_input, " ", "", &inputNoWhitespace);
  return HexStringToBytesT(inputNoWhitespace, _output);
}

bool
HexStringToASCIIString(const std::string &_input, std::string &_output)
{
  std::vector<uint8> output_vec;
  bool result = HexStringToBytes(_input, &output_vec);
  _output = std::string(output_vec.begin(), output_vec.end());
  return result;
}

void
ByteToHexString(const uint8 _input, std::string *_output)
{
  uint8 msb = 0;  // most significant 4 bits
  uint8 lsb = 0;  // least significant 4 bits

  msb = _input & 0xF0;
  msb = msb >> 4;
  lsb = _input & 0x0F;

  char m, l;

  if (msb < 10)
    m = msb + '0';    
  else
    m = msb + 'A' - 10;

  if (lsb < 10)
    l = lsb + '0';    
  else
    l = lsb + 'A' - 10;

  (*_output).resize(2);
  (*_output)[0] = m;
  (*_output)[1] = l;

  FormatHexString(*_output);
}

void
BytesToHexString(const std::vector<uint8> &_input, std::string *_output)
{
  *_output = "";
  for (int i = 0; i < _input.size(); ++i)
  {
    std::string result = "";
    ByteToHexString(_input[i], &result);
    *_output += result;
  }

  FormatHexString(*_output);
}

void
ASCIIStringToHexString(const std::string &_input, std::string *_output)
{
  const std::vector<uint8> input_vec(_input.begin(), _input.end());
  BytesToHexString(input_vec, _output);

  FormatHexString(*_output);
}

void
UintToHexString(const unsigned int _input, std::string *_output)
{
  std::stringstream stream;
  stream << std::uppercase << std::hex << _input;
  std::string temp = "";
  stream >> temp;

  if (0 != temp.length() % 2)
  {
    temp = "0" + temp;
  }

  (*_output) = temp;

  FormatHexString(*_output);
}


}; // namespace util
