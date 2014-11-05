
#ifndef __EXCEPTION_HPP__
#define __EXCEPTION_HPP__

#include <iostream>
#include <string>

class Exception
{
public:
  Exception(const char *str)
  {
    message = std::string(str);
  }

  void print()
  {
    std::cout << message << std::endl;
  }

private:
  std::string message;
};

#endif
