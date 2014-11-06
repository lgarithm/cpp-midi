
#ifndef __SEQUENCE_HPP__
#define __SEQUENCE_HPP__

#include <string>
#include <list>
#include "Note.hpp"

class Sequence
{
public:
  Sequence(const std::string &score): channel(0), program_number(0)
  {
    NoteReader in(score);
    for (Note *note; (note = in.get_note()) != NULL; ) notes.push_back(note);
  }

  void set_channel(unsigned char chan)
  {
    channel = chan;
  }

  void set_program_number(unsigned char prog_num)
  {
    program_number = prog_num;
  }

  void delay(unsigned short beats)
  {
    notes.push_front(new PauseNote(beats * midi::beat));
  }

  midi_track to_track()
  {
    return to_track(channel, program_number);
  }

  midi_track to_track(unsigned char chan)
  {
    return to_track(chan, program_number);
  }

  midi_track to_track(unsigned char chan, unsigned prog_num)
  {
    midi_track track;

    // track.add_event(new controller(0, 0, chan));
    // track.add_event(new controller(32, 0, chan));
    track.add_event(new program_change(prog_num, chan));

    unsigned dlt = 0;
    for (auto it : notes) {
      track.add_event(it->note_on_event(dlt, chan));
      track.add_event(it->note_off_event(dlt, chan));
    }

    // track.add_event(new controller(127, 0, chan));
    return track;
  }

private:
  std::list<Note *> notes;
  unsigned char channel;
  unsigned char program_number;
};

#endif
