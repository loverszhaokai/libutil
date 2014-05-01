/******************************************************************************
 
  libutil
  
  Author: zhaokai
  
  Email: loverszhao@gmail.com

  Reference: POSIX

  Description:

  Version: 1.0

******************************************************************************/

#ifndef UTIL_REGEX_UTIL_H_
#define UTIL_REGEX_UTIL_H_

#include <string>
#include <vector>

namespace util
{
  // @_pattern is regex expression
  // @_input is the destination string
  // @_output contains the substring that matched
  // @_err_msg is set the error message when return false
  //
  // Return false if @_pattern is invalid
  // true otherwise
  // 
  bool RegexMatch(const std::string &_pattern,
                  const std::string &_input,
                  std::vector<std::string> *_output,
                  std::string *_err_msg);

  // Return true if @_input and @_pattern are full-matched
  // flase otherwise
  bool FullMatch(const std::string &_pattern,
                 const std::string &_input);
  
}; // namespace util

#endif
