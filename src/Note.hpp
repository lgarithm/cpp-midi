#pragma once

#include <cstring>
#include <queue>
#include <vector>

#include "event.hpp"
#include "varlength.hpp"

class TimeLine
{

  private:
};

class Note
{
  public:
    Note(unsigned length) : length(length) {}

    virtual event *note_on_event(unsigned &dlt, unsigned char chan) = 0;
    virtual event *note_off_event(unsigned &dlt, unsigned char chan) = 0;

  protected:
    unsigned length;
};

class MusicNote : public Note
{
  public:
    MusicNote(unsigned length, unsigned char pitch) : Note(length), pitch(pitch)
    {
    }

    event *note_on_event(unsigned &dlt, unsigned char chan)
    {
        midi_event *e = new note_on(pitch, midi::on_velo, chan);
        e->set_delta_time(dlt);
        return e;
    }

    event *note_off_event(unsigned &dlt, unsigned char chan)
    {
        midi_event *e = new note_off(pitch, midi::off_velo, chan);
        e->set_delta_time(length);
        dlt = 0;
        return e;
    }

  private:
    unsigned char pitch;
};

class PauseNote : public Note
{
  public:
    PauseNote(unsigned length) : Note(length) {}

    event *note_on_event(unsigned &dlt, unsigned char chan) { return NULL; }

    event *note_off_event(unsigned &dlt, unsigned char chan)
    {
        dlt += length;
        return NULL;
    }
};

class Chord
{
  public:
  private:
};

class NoteReader
{
  public:
    NoteReader(const std::string &s)
        : buffer(s), iter(buffer.begin()), buffer_end(buffer.end())
    {
        for (char ch : "[]<>!?") count[ch] = 0;
    }

    Note *get_note()
    {
        for (char ch : "-+b#_^.") count[ch] = 0;
        while (iter != buffer_end) {
            char ch = *iter++;
            if (ch == '$') return NULL;
            if ('1' <= ch && ch <= '7') {
                int p = pitch(ch);
                if (0 <= p and p <= 127) return new MusicNote(length(), p);
                fprintf(stderr, "Bad note, out of range!\n");
                return NULL;
            }
            if (ch == 's' or ch == '0') return new PauseNote(length());
            ++count[ch];
        }
        return NULL;
    }

  private:
    int pitch(char ch)
    {
        return basic_note_number[ch - '1'] +
               (count['+'] - count['-'] + count['>'] - count['<']) * 12 +
               count['#'] - count['b'] + count['?'] - count['!'];
    }

    int length()
    {
        int shift = count['^'] - count['_'] - count['['] + count[']'];
        int length = shift > 0 ? midi::beat << shift : midi::beat >> -shift;
        for (int x = length >> 1; count['.']-- > 0; x >>= 1) length += x;
        return length;
    }

  private:
    char count[256];
    const std::string buffer;
    std::string::const_iterator iter;
    std::string::const_iterator buffer_end;
};
