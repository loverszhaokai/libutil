/******************************************************************************
 
  libutil
  
  Author: zhaokai
  
  Email: loverszhao@gmail.com

  Reference: chromium

  Description:

  Version: 1.0

******************************************************************************/

#ifndef UTIL_STRING_NUMBER_CONVERSIONS_H_
#define UTIL_STRING_NUMBER_CONVERSIONS_H_

#include <sstream>
#include <string>
#include <vector>

#include "util/basictypes.h"

namespace util
{

bool StringToInt(const std::string &_input, int *_output);
bool StringToUint(const std::string &_input, unsigned *_output);
bool StringToInt64(const std::string &_input, int64 *_output);
bool StringToUint64(const std::string &_input, uint64 *_output);
bool StringToSizeT(const std::string &_input, size_t *_output);

template <typename VALUE>
bool StringToIntImpl(const std::string &_input, VALUE *_output);

std::string HexEncode(const void* bytes, size_t size);
bool HexStringToInt(const std::string &_input, int *_output);
bool HexStringToBytes(const std::string& input, std::vector<uint8>* output);
bool HexStringToASCIIString(const std::string &_input, std::string &_output);
void ByteToHexString(const uint8 _input, std::string *_output);
void BytesToHexString(const std::vector<uint8> &_input, std::string *_output);
void ASCIIStringToHexString(const std::string &_input, std::string *_output);
void UintToHexString(const unsigned int _input, std::string *_output);

template <typename T>
    std::string ToString(const T &_t)
{
  std::ostringstream oss;
  oss << _t;
  return oss.str();
}



}; // namespace util


#endif
