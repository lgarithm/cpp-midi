
#ifndef __COMPOSER_HPP__
#define __COMPOSER_HPP__

#include <cstdio>
#include "midiFile.hpp"
#include "Sequence.hpp"

class Composer
{
public:
  midiFile compose()
  {
    return compose(stdin);
  }
  midiFile compose(FILE *fp)
  {
    ss.clear();
    buffer.clear();

    char ch;
    while ((ch = fgetc(fp)) != EOF)
      {
	if (ch == '$')
	  {
	    ss.push_back(buffer);
	    buffer.clear();
	  }
	else
	  {
	    buffer.push_back(ch);
	  }
      }
    if (!buffer.empty()) ss.push_back(buffer);

    midiFile midi;
    midi.add_track(midi_control_track());

    int tc = 0;
    for (std::list<std::string>::iterator it = ss.begin(); it != ss.end(); ++it)
      midi.add_track(Sequence(*it).to_track(tc++));

    return midi;
  }
private:
  std::list<std::string> ss;
  std::string buffer;
};

#endif
