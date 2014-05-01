/******************************************************************************
 
  libutil
  
  Author: zhaokai
  
  Email: loverszhao@gmail.com

  Reference: chromium

  Description:

  Version: 1.0

******************************************************************************/

#include <iostream>
#include <string>

#include "util/string_number_conversions.h"

int main(int argc, char *argv[])
{
  const std::string input = "123456789";
  int output = 0;
  if (util::StringToInt(input, &output))
  {
    std::cout << "StringToInt is true" << std::endl;
    std::cout << "int is=" << output << std::endl;
  } 

  return 0;
}

