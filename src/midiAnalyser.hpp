
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
		
		std::cout<<"format: "<<head.format<<std::endl;
		std::cout<<"ntrks: "<<head.ntrks<<std::endl;
		std::cout<<"division: "<<head.division<<std::endl;
		std::cout<<std::endl;
		for (int track_id=0; track_id < head.ntrks; ++track_id)
		{
			std::cout<<"track "<<track_id + 1<<", length: "<<tracks[track_id].length<<std::endl;
			
			int count = 0;
			for (std::list<event *>::const_iterator it = tracks[track_id].events.begin();
				it != tracks[track_id].events.end(); ++it)
			{
				std::cout<<std::setw(5)<<++count<<": ";
				std::cout<<(*it)->to_text();
				std::cout<<std::endl;
			}
			std::cout<<std::endl;
		}
	}
};

#endif
