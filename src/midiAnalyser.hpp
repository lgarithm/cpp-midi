
#ifndef __MIDIANALYSER_HPP__
#define __MIDIANALYSER_HPP__

#include <iostream>
#include <iomanip>
#include "event.hpp"
#include "midiFile.hpp"

class midiAnalyser
{
public:
  void analyse(const midiFile &file)
  {
    midi_head head = file.get_head();
    std::vector<midi_track> tracks = file.get_tracks();

    std::cout << "format: "   << head.format   << std::endl
	      << "ntrks: "    << head.ntrks    << std::endl
	      << "division: " << head.division << std::endl
	      << std::endl;

    for (int track_id=0; track_id < head.ntrks; ++track_id) {
      std::cout << "track " << track_id + 1
		<< ", length: " << tracks[track_id].length
		<< std::endl;

      int count = 0;
      for (auto &it : tracks[track_id].events)
	std::cout << std::setw(5) << ++count << ": "
		  << it->to_text()
		  << std::endl;

      std::cout<<std::endl;
    }
  }
};

#endif
