
#ifndef __MIDIFILE_HPP__
#define __MIDIFILE_HPP__

#include <cstring>
#include <fstream>
#include "event.hpp"
#include "byte.hpp"
#include "Trace.hpp"

struct midi_chunk
{
  char          magic[4];
  unsigned      length;

  void write(std::ofstream &file) const
  {
    file.write(magic, 4);
    unsigned buffer = length;
    reverse_byte_order(buffer);
    file.write((char *) &buffer, 4);
  }
};

struct midi_head : public midi_chunk
{
  unsigned short        format;
  unsigned short        ntrks;
  unsigned short        division;

  midi_head()
  {
    strncpy(magic, "MThd", 4);
    length = 6;
    format = 0;
    ntrks = 0;
    division = 960;
  }

  void write(std::ofstream &file) const
  {
    file.write(magic, 4);
    char buffer[10];
    buffer[0] = 0;
    buffer[1] = 0;
    buffer[2] = 0;
    buffer[3] = 6;
    buffer[4] = format >> 8;
    buffer[5] = format & 0xFF;
    buffer[6] = ntrks >> 8;
    buffer[7] = ntrks & 0xFF;
    buffer[8] = division >> 8;
    buffer[9] = division & 0xFF;
    file.write(buffer, 10);
  }
};

class midi_track : public midi_chunk
{
public:
  midi_track()
  {
    strncpy(magic, "MTrk", 4);
    length = 0;
    set_end_of_track = false;
  }

  void write(std::ofstream &file)
  {
    std::string data;
    for (auto it : events) data += it->to_binary();
    if (not set_end_of_track) data += end_of_track().to_binary();
    length = data.size();

    file.write(magic, 4);
    unsigned buffer = length;
    reverse_byte_order(buffer);
    file.write((char*) &buffer, 4);
    file.write(data.c_str(), length);
  }

  void add_event(event* e)
  {
    if (e != NULL) {
      if (set_end_of_track) warning.print("warning: end_of_track event has been set.");
      if (e->get_status_byte() == 0xFF) {
	meta_event *m = (meta_event *) e;
	if (m->get_status_byte() == 0x2F) {
	  set_end_of_track = true;
	}
      }
      events.push_back(e);
    }
  }

  std::list<event*> events;

private:
  bool set_end_of_track;
};

midi_track midi_control_track()
{
  midi_track track;
  track.add_event(new set_tempo(500000));
  track.add_event(new time_signature(0x04, 0x02, 0x02, 0x08));
  return track;
}

class midiFile
{
public:
  midiFile() {}

  midiFile(midi_head head, std::vector<midi_track> tracks): head(head), tracks(tracks) {}

  void add_track(midi_track track)
  {
    if (++head.ntrks > 1) head.format = 1;
    tracks.push_back(track);
  }

  void write_file(const char * const filepath)
  {
    std::ofstream file(filepath, std::ios::out | std::ios::binary);
    head.write(file);
    for (int i=0; i < head.ntrks; ++i) tracks[i].write(file);
    file.close();
  }

  midi_head get_head() const
  {
    return head;
  }

  std::vector<midi_track> get_tracks() const
  {
    return tracks;
  }

private:
  midi_head head;
  std::vector<midi_track> tracks;
};

#endif
