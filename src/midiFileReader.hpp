
#ifndef __MIDIFILEREADER_HPP__
#define __MIDIFILEREADER_HPP__

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include "event.hpp"
#include "midiFile.hpp"
#include "varlength.hpp"
#include "byte.hpp"
#include "Trace.hpp"
#include "Exception.hpp"

class midiFileReader
{
public:
  midiFile read(std::string filepath)
  {
    std::ifstream file(filepath.c_str(), std::ios::in | std::ios::binary);
    if (not file) throw Exception("can not open file exception.");
    if (not check_midi_file(file)) throw Exception("bad midi file exception.");
    file.seekg(0, std::ios::beg);

    midi_head head;
    file.read((char*) &head, 14);
    reverse_byte_order(head.length);
    reverse_byte_order(head.format);
    reverse_byte_order(head.ntrks);
    reverse_byte_order(head.division);

#ifdef __DEBUG__
    std::cout<<"format: "<<head.format<<std::endl;
    std::cout<<"ntrks: "<<head.ntrks<<std::endl;
    std::cout<<"division: "<<head.division<<std::endl;
#endif

    std::vector<midi_track> tracks(head.ntrks);
    for (int track_id=0; track_id < head.ntrks; ++track_id)
      {
	file.read((char*) &tracks[track_id], 8);
	reverse_byte_order(tracks[track_id].length);

#ifdef __DEBUG__
	std::cout<<"track "<<track_id + 1<<": length: "<<tracks[track_id].length<<std::endl;
#endif
	get_events(file, tracks[track_id]);
      }

    file.close();

    return midiFile(head, tracks);
  }
private:
  bool check_midi_file(std::ifstream &file)
  {
    file.seekg(0, std::ios::end);
    std::streampos end_pos = file.tellg();
    file.seekg(0, std::ios::beg);
    unsigned rest = end_pos;
    bool first_chunk = true;
    while (file.tellg() != end_pos)
      {
	midi_chunk chunk;
	if (!file.read((char*) &chunk, 8)) return false;
	if (first_chunk)
	  {
	    first_chunk = false;
	    if (strncmp(chunk.magic, "MThd", 4) != 0) return false;
	  }
	else
	  {
	    if (strncmp(chunk.magic, "MTrk", 4) != 0) return false;
	  }

	unsigned length = get_reversed_value(chunk.length);
	if (length > rest) return false;
	rest -= length;

	file.seekg(length, std::ios::cur);
	if (file.fail()) return false;
      }

    return true;
  }
  bool check_midi_file_track();

  void get_events(std::ifstream &file, midi_track &track)
  {
    unsigned char* byte = new unsigned char[track.length];
    file.read((char*) byte, track.length);

    unsigned char status_byte = 0;

#ifdef __DEBUG__
    int cc = 0;
#endif

    for (const unsigned char* p = byte; p < byte + track.length; )
      {
#ifdef __DEBUG__
	std::cout<<++cc<<":";
#endif
	unsigned delta_time = read_variable_length(p);
	bool running = false;
	p[0] < 0x80 ? running = true : status_byte = *p++;
	if (status_byte < 0x80) throw Exception("status byte exception 1: status byte < 0x80.");

	event* e = NULL;
	switch (status_byte)
	  {
	  case 0xF0:
	  case 0xF7: e = get_sysex_event(status_byte, p);
	    break;
	  case 0xFF: e = get_meta_event(status_byte, p);
	    break;
	  default: e = get_midi_event(status_byte, p);
	    break;
	  }
	if (running) e->running_on();
	e->set_delta_time(delta_time);

	track.add_event(e);

#ifdef __DEBUG__
	std::cout<<std::endl;
#endif
      }

    delete byte;
  }

  midi_event* get_midi_event(unsigned char status_byte, const unsigned char *&p)
  {
    // std::cout<<"(midi event)";
    unsigned char type = status_byte & 0xF0;
    unsigned char chan = status_byte & 0x0F;

    midi_event *e = NULL;
    switch (type)
      {
      case 0x80: e = new note_off(p[0], p[1]);
	break;
      case 0x90: e = new note_on(p[0], p[1]);
	break;
      case 0xA0: e = new note_aftertouch(p[0], p[1]);
	break;
      case 0xB0: e = new controller(p[0], p[1]);
	break;
      case 0xC0: e = new program_change(p[0]);
	break;
      case 0xD0: e = new channel_aftertouch(p[0]);
	break;
      case 0xE0: e = new pitch_bend(p[0], p[1]);
	break;

      default:
	throw Exception("status byte exception 2: unknown status byte 0xF[12345689ABCDE] exception.");
	break;
      }

    e->set_channel(chan);
    p += e->get_length();
    return e;
  }


  sysex_event* get_sysex_event(unsigned char status_byte, const unsigned char *&p)
  {
    // std::cout<<"(sysex event)";
    unsigned length = read_variable_length(p);
    sysex_event* e = NULL;
    //* todo
    // switch (status_byte)
    // {
    // case 0xF0:
    // case 0xF7;
    // default:
    // break;
    // }
    e = new sysex_event(status_byte, length, p);

    p += length;
    return e;
  }

  meta_event* get_meta_event(unsigned char status_byte, const unsigned char *&p)
  {
    // std::cout<<"(meta event)";
    unsigned char type = *p++;
    unsigned length = read_variable_length(p);
    meta_event* e = NULL;

    switch (type)
      {
      case 0x00: e = new sequence_number(p[0], p[1]);
	break;
      case 0x01: e = new text_event(length, p);
	break;
      case 0x02: e = new copyriht_notice(length, p);
	break;
      case 0x03: e = new sequence_or_track_name(length, p);
	break;
      case 0x04: e = new instrument_name(length, p);
	break;
      case 0x05: e = new lyrics(length, p);
	break;
      case 0x06: e = new marker(length, p);
	break;
      case 0x07: e = new cue_point(length, p);
	break;
      case 0x20: e = new midi_channel_prefix(p[0]);
	break;
      case 0x2F: e = new end_of_track();
	break;
      case 0x51: e = new set_tempo(p);
	break;
      case 0x54: e = new smpte_offset(p);
	break;
      case 0x58: e = new time_signature(p[0], p[1], p[2], p[3]);
	break;
      case 0x59: e = new key_signature(p[0], p[1]);
	break;
      case 0x7F: e = new sequence_specific(length, p);
	break;

      default: e = new unknown_meta_event(type, length, p);
	warning.print("warning: meta event type exception");
	break;
      }

    p += length;
    return e;
  }
};

#endif
