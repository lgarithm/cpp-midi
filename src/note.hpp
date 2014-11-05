
#ifndef __NOTE_HPP__
#define __NOTE_HPP__

#include <cstring>
#include <vector>
#include <queue>
#include "event.hpp"
#include "varlength.hpp"

class TimeLine
{

private:

};

class Note
{
public:
	virtual event* note_on_event(unsigned &dlt, unsigned char chan) = 0;
	virtual event* note_off_event(unsigned &dlt, unsigned char chan) = 0;
protected:
	unsigned length;
};

class MusicNote : public Note
{
public:
	MusicNote(unsigned length, unsigned char pitch)
	{
		this->length = length;
		this->pitch = pitch;
	}
	event* note_on_event(unsigned &dlt, unsigned char chan)
	{
		midi_event *e = new note_on(pitch, midi::on_velo, chan);
		e->set_delta_time(dlt);
		return e;
	}
	event* note_off_event(unsigned &dlt, unsigned char chan)
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
	PauseNote(unsigned length)
	{
		this->length = length;
	}
	event* note_on_event(unsigned &dlt, unsigned char chan)
	{
		return NULL;
	}
	event* note_off_event(unsigned &dlt, unsigned char chan)
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
	NoteReader(const char * str)
	{
		count['?'] = 0;
		count['!'] = 0;
		count['>'] = 0;
		count['<'] = 0;
		int n = strlen(str);
		for (int i=0; i < n; ++i) buffer.push(str[i]);
		buffer.push(0);
	}
	Note *get_note()
	{
		count['^'] = 0;
		count['_'] = 0;
		count['.'] = 0;
		count['#'] = 0;
		count['+'] = 0;
		count['-'] = 0;

		register char ch;
		while ((ch = next_char()) != 0)
		{
			if (ch == '$') return NULL;

			if ('1' <= ch && ch <= '7')
			{
				int pitch = basic_note_number[ch - '1']
						+ (count['+'] - count['-'] + count['?'] - count['!']) * 12
						+ count['#'] + count['>'] - count['<'];
				if (pitch < 0 || 127 < pitch)
				{
					fprintf(stderr, "Bad note, out of range!\n");
					return NULL;
				}
				int length = (midi::beat << count['^']) >> count['_'];
				for (int x = length >> 1; count['.']-- > 0; x >>= 1) length += x;
				return new MusicNote(length, pitch);
			}

			if (ch == 's')
			{
				int length = (midi::beat << count['^']) >> count['_'];
				for (int x = length >> 1; count['.']-- > 0; x >>= 1) length += x;
				return new PauseNote(length);
			}

			++count[ch];
		}
	}
private:
	char next_char()
	{
		char tmp = buffer.front();
		buffer.pop();
		return tmp;
	}
private:
	char count[256];
	std::queue<char> buffer;
};

#endif
