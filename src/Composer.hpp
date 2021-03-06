#pragma once

#include <algorithm>
#include <iostream>
#include <iterator>

#include "Sequence.hpp"
#include "midiFile.hpp"

class Composer
{
  public:
    Composer(unsigned char bpm) : bpm(bpm) {}

    midiFile compose(const char *filename)
    {
        std::ifstream in(filename);
        return compose(in);
    }

    midiFile compose(std::istream &in = std::cin)
    {
        std::list<std::string> ss;
        std::string buffer;

        std::for_each(std::istream_iterator<char>(in),
                      std::istream_iterator<char>(), [&](char ch) {
                          if (ch == '$') {
                              ss.push_back(buffer);
                              buffer.clear();
                          } else {
                              buffer.push_back(ch);
                          }
                      });
        if (!buffer.empty()) ss.push_back(buffer);

        midiFile midi;
        midi.add_track(midi_control_track(bpm));

        int tc = 0;
        for (auto it : ss) { midi.add_track(Sequence(it).to_track(tc++)); }
        return midi;
    }

  private:
    const unsigned char bpm;
};
