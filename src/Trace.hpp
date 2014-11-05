
#ifndef __TRACE_HPP__
#define __TRACE_HPP__

#include <cstdio>

class Trace
{
public:
  Trace()
  {
    noisy = 0;
    f = stdout;
  }
  Trace(FILE *ff)
  {
    noisy = 1;
    f = ff;
  }
  void print(const char *s)
  {
    if (noisy)
      {
        fprintf(f, "%s\n", s);
      }
  }
  void on()
  {
    noisy = 1;
  }
  void off()
  {
    noisy = 0;
  }
private:
  int noisy;
  FILE *f;
};

static Trace warning(stderr);
static Trace debug(stderr);

#endif
