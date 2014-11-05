
#ifndef __COMPOSER_HPP__
#define __COMPOSER_HPP__

#include <iostream>
#include "midiFile.hpp"
#include "Sequence.hpp"

class Composer
{
public:
  midiFile compose(const char* filename)
  {
    std::ifstream in(filename);
    return compose(in);
  }

  midiFile compose(std::istream& in = std::cin)
  {
    std::list<std::string> ss;
    std::string buffer;

    for_each(std::istream_iterator<char>(in),
	     std::istream_iterator<char>(),
	     [&](char ch){
	       if (ch == '$') {
		 ss.push_back(buffer);
		 buffer.clear();
	       }
	       else {
		 buffer.push_back(ch);
	       }
	     });
    if (!buffer.empty()) ss.push_back(buffer);

    midiFile midi;
    midi.add_track(midi_control_track());

    int tc = 0;
    for (auto it : ss) {
      midi.add_track(Sequence(it).to_track(tc++));
    }
    return midi;
  }
};

#endif
