/******************************************************************************
 
  libutil
  
  Author: zhaokai
  
  Email: loverszhao@gmail.com

  Reference: POSIX

  Description:

  Version: 1.0

******************************************************************************/

#include "util/regex_util.h"

#include <algorithm>
#include <climits>
#include <iostream>
#include <regex.h>

namespace util
{

bool
RegexMatch(const std::string &_pattern,
           const std::string &_input,
           std::vector<std::string> *_output,
           std::string *_err_msg)
{
  _output->clear();
  *_err_msg = "";
  regex_t reg;

  int comp_result = regcomp(&reg, _pattern.c_str(), REG_EXTENDED | REG_NEWLINE);
  if (0 == comp_result)
  {
    // Add 1 in case of "" == _input
    size_t nmatch = 1 + std::min(_input.length(), UINT_MAX - 1);
    regmatch_t pmatch[nmatch];
    int exec_result = regexec(&reg, _input.c_str(), nmatch, pmatch, 0);

    if (0 == exec_result)
    {
      for (size_t i = 0; i < nmatch; ++i)
      {
        if (-1 == pmatch[i].rm_so)
          break;

        _output->push_back(_input.substr(pmatch[i].rm_so, pmatch[i].rm_eo));
      }
    }
  }
  else
  {
    const size_t errbuf_size = 1000;
    char errbuf[errbuf_size + 1];
    regerror(comp_result, &reg, errbuf, errbuf_size);
    *_err_msg = errbuf;
    
    regfree(&reg);
    return false;
  }

  regfree(&reg);
  return true;
}

bool
FullMatch(const std::string &_pattern,
          const std::string &_input)
{
  std::vector<std::string> output;
  std::string err_msg = "";
  bool result = RegexMatch(_pattern, _input, &output, &err_msg);

  if (!result)
  {
    std::cout << err_msg << std::endl;
    return false;
  }
  else
  {
    // full-match in case of @_input == output[0]
    if (0 < output.size() && _input == output[0])
      return true;
  }
  return false;
}

}; // namespace util
