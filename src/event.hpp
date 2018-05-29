#pragma once

#include <string>

#include "midi.hpp"
#include "varlength.hpp"

class event
{
  public:
    event() : running(false), delta_time(0) {}

    void running_on() { running = true; }

    void set_delta_time(unsigned dlt) { delta_time = dlt; }

    unsigned get_delta_time() const { return delta_time; }

    virtual std::string to_text() const { return "event"; }

  public:
    virtual unsigned get_length() const = 0;
    virtual std::string to_binary() const = 0;
    virtual unsigned char get_status_byte() const = 0;

  protected:
    unsigned delta_time;
    bool running;
};

class midi_event : public event
{
  public:
    midi_event() : channel(0) {}
    midi_event(unsigned char chan) : channel(chan) {}

    void set_channel(unsigned char chan) { channel = chan; }

    std::string to_text() const { return "midi event"; }

  protected:
    unsigned char channel;
};

class unary_midi_event : public midi_event
{
  public:
    unary_midi_event(unsigned char param) : parameter(param) {}
    unary_midi_event(unsigned char param, unsigned char chan)
        : parameter(param), midi_event(chan)
    {
    }

    std::string to_text() const { return "unary midi event"; }

    std::string to_binary() const
    {
        unsigned char byte[8];
        unsigned char *p = byte;
        write_variable_length(delta_time, p);
        if (not running) *p++ = get_status_byte() | (channel & 0x0F);
        *p++ = parameter;
        return std::string(byte, p);
    };

    unsigned get_length() const { return 1; }

    void set_parameter(unsigned char param) { parameter = param; }

  protected:
    unsigned char parameter;
};

class binary_midi_event : public midi_event
{
  public:
    binary_midi_event(unsigned char param1, unsigned char param2)
        : first_parameter(param1), second_parameter(param2)
    {
    }
    binary_midi_event(unsigned char param1, unsigned char param2,
                      unsigned char chan)
        : first_parameter(param1), second_parameter(param2), midi_event(chan)
    {
    }

    std::string to_text() const { return "binary midi event"; }

    std::string to_binary() const
    {
        unsigned char byte[8];
        unsigned char *p = byte;
        write_variable_length(delta_time, p);
        if (not running) *p++ = get_status_byte() | (channel & 0x0F);
        *p++ = first_parameter;
        *p++ = second_parameter;
        return std::string(byte, p);
    };

    unsigned get_length() const { return 2; }

    void set_first_parameter(unsigned char param) { first_parameter = param; }

    void set_second_parameter(unsigned char param) { second_parameter = param; }

    void set_parameters(unsigned char param1, unsigned char param2)
    {
        first_parameter = param1;
        second_parameter = param2;
    }

  protected:
    unsigned char first_parameter;
    unsigned char second_parameter;
};

class note_off : public binary_midi_event
{
  public:
    note_off(unsigned char note_number, unsigned char velocity)
        : binary_midi_event(note_number, velocity)
    {
    }
    note_off(unsigned char note_number, unsigned char velocity,
             unsigned char channel)
        : binary_midi_event(note_number, velocity, channel)
    {
    }

    std::string to_text() const
    {
        char str[260];
        sprintf(str,
                "(%c)DT: %5d,   chan: 0x%02X,   note %3d(%3s) off,   velo: %3d",
                running ? 'r' : '*', delta_time, channel, first_parameter,
                note_name[first_parameter], second_parameter);
        return str;
    }

    unsigned char get_note_number() const { return first_parameter; }

    unsigned char get_velocity() const { return second_parameter; }

    unsigned char get_status_byte() const { return 0x80; }
};

class note_on : public binary_midi_event
{
  public:
    note_on(unsigned char note_number, unsigned char velocity)
        : binary_midi_event(note_number, velocity)
    {
    }
    note_on(unsigned char note_number, unsigned char velocity,
            unsigned char channel)
        : binary_midi_event(note_number, velocity, channel)
    {
    }

    std::string to_text() const
    {
        char str[260];
        sprintf(str,
                "(%c)DT: %5d,   chan: 0x%02X,   note %3d(%3s)  on,   velo: %3d",
                running ? 'r' : '*', delta_time, channel, first_parameter,
                note_name[first_parameter], second_parameter);
        return str;
    }

    unsigned char get_note_number() const { return first_parameter; }

    unsigned char get_velocity() const { return second_parameter; }

    unsigned char get_status_byte() const { return 0x90; }
};

class note_aftertouch : public binary_midi_event
{
  public:
    note_aftertouch(unsigned char note_number, unsigned char aftertouch_value)
        : binary_midi_event(note_number, aftertouch_value)
    {
    }

    std::string to_text() const { return "note aftertouch"; }

    unsigned char get_note_number() const { return first_parameter; }

    unsigned char get_aftertouch_value() const { return second_parameter; }

    unsigned char get_status_byte() const { return 0xA0; }
};

class controller : public binary_midi_event
{
  public:
    controller(unsigned char controller_number, unsigned char controller_value)
        : binary_midi_event(controller_number, controller_value)
    {
    }
    controller(unsigned char controller_number, unsigned char controller_value,
               unsigned char channel)
        : binary_midi_event(controller_number, controller_value, channel)
    {
    }

    std::string to_text()
    {
        char str[260];
        sprintf(str, "(%c)DT: %5d,   chan: 0x%02X,   ctl %3d,   value: %3d",
                running ? 'r' : '*', delta_time, channel, first_parameter,
                second_parameter);
        return str;
    }

    unsigned char get_controller_number() const { return first_parameter; }

    unsigned char get_controller_value() const { return second_parameter; }

    unsigned char get_status_byte() const { return 0xB0; }
};

class program_change : public unary_midi_event
{
  public:
    program_change(unsigned char program_number)
        : unary_midi_event(program_number)
    {
    }
    program_change(unsigned char program_number, unsigned char channel)
        : unary_midi_event(program_number, channel)
    {
    }

    std::string to_text() const
    {
        char str[260];
        sprintf(
            str,
            "(%c)DT: %5d,   chan: 0x%02X,   program change, program number: %d",
            running ? 'r' : '*', delta_time, channel, parameter);
        return str;
    }

    unsigned char get_program_number() const { return parameter; }

    unsigned char get_status_byte() const { return 0xC0; }
};

class channel_aftertouch : public unary_midi_event
{
  public:
    channel_aftertouch(unsigned char aftertouch_value)
        : unary_midi_event(aftertouch_value)
    {
    }

    std::string to_text() const { return "channel aftertouch"; }

    unsigned char get_aftertouch_value() const { return parameter; }

    unsigned char get_status_byte() const { return 0xD0; }
};

class pitch_bend : public binary_midi_event
{
  public:
    pitch_bend(unsigned char pitch_value_LSB, unsigned char pitch_value_MSB)
        : binary_midi_event(pitch_value_LSB, pitch_value_MSB)
    {
    }

    std::string to_text() const { return "pitch bend"; }

    unsigned char get_pitch_value_LSB() const { return first_parameter; }

    unsigned char get_pitch_value_MSB() const { return second_parameter; }

    unsigned char get_status_byte() const { return 0xE0; }
};

class sysex_event : public event  // 0xF0, 0xF7
{
  public:
    sysex_event(unsigned char status_byte, unsigned length,
                const unsigned char *const p)
        : status_byte(status_byte), data(std::string(p, p + length))
    {
    }

    std::string to_text() const { return "sysex event"; }

    std::string to_binary() const
    {
        unsigned char byte[16];
        unsigned char *p = byte;

        write_variable_length(delta_time, p);
        if (not running) *p++ = get_status_byte();
        unsigned length = data.size();
        write_variable_length(length, p);

        return std::string(byte, p) + data;
    }

    unsigned get_length() const { return data.size(); }

    unsigned char get_status_byte() const { return status_byte; }

  protected:
    unsigned char status_byte;
    std::string data;
};

/*
  class normal_sysex_event : public sysex_event //0xF0, the data ends with 0xF7
  {
  private:
  unsigned char get_status_byte()
  {
  return 0xF0;
  }
  };

  class divided_sysex_event_head : public sysex_event   //0xF0
  {
  private:
  unsigned char get_status_byte()
  {
  return 0xF0;
  }
  };

  class divided_sysex_event_body : public sysex_event   //0xF7
{
  private:
  unsigned char get_status_byte()
  {
  return 0xF7;
  }
  };

  class divided_sysex_event_tail : public sysex_event   //0xF7, the data ends
with 0xF7
{
  private:
  unsigned char get_status_byte()
  {
  return 0xF7;
  }
  };

  class authorization_sysex_event : public sysex_event  //0xF7
  {
  private:
  unsigned char get_status_byte()
  {
  return 0xF7;
  }
  };
//*/

class meta_event : public event
{
  public:
    std::string to_binary() const
    {
        unsigned char byte[16];
        unsigned char *p = byte;

        write_variable_length(delta_time, p);
        if (not running) *p++ = get_status_byte();
        *p++ = get_type();
        write_variable_length(get_length(), p);

        return std::string(byte, p) + get_data();
    }

    std::string to_text() const
    {
        char str[260];
        sprintf(str,
                "(%c)DT: %5d,   meta event,   type: 0x%02X(%s),   length: %d",
                running ? 'r' : '*', delta_time, get_type(),
                meta_event_type_name[get_type()], get_length());
        return str;
    }

    unsigned char get_status_byte() const { return 0xFF; }

    virtual unsigned char get_type() const = 0;

  protected:
    virtual std::string get_data() const = 0;
};

class ascii_text_meta_event : public meta_event
{
  public:
    ascii_text_meta_event(unsigned length, const unsigned char *const p)
        : data(std::string(p, p + length))
    {
    }

    unsigned get_length() const { return data.size(); }

  protected:
    std::string get_data() const { return data; }

  private:
    std::string data;
};

class unknown_meta_event : public meta_event
{
  public:
    unknown_meta_event(unsigned char type, unsigned length,
                       const unsigned char *const p)
        : type(type), data(std::string(p, p + length))
    {
    }

    unsigned char get_type() const { return type; }

    unsigned get_length() const { return data.size(); }

  protected:
    std::string get_data() const { return data; }

  private:
    std::string data;
    unsigned char type;
};

class sequence_number : public meta_event
{
  public:
    sequence_number(unsigned char msb, unsigned char lsb) : MSB(msb), LSB(lsb)
    {
    }

    unsigned char get_type() const { return 0x00; }

    unsigned get_length() const { return 2; }

  private:
    std::string get_data() const
    {
        unsigned char byte[2] = {MSB, LSB};
        return std::string(byte, byte + 2);
    }

  private:
    unsigned char MSB;
    unsigned char LSB;
};

class text_event : public ascii_text_meta_event
{
  public:
    text_event(unsigned length, const unsigned char *const p)
        : ascii_text_meta_event(length, p)
    {
    }

    unsigned char get_type() const { return 0x01; }
};

class copyright_notice : public ascii_text_meta_event
{
  public:
    copyright_notice(unsigned length, const unsigned char *const p)
        : ascii_text_meta_event(length, p)
    {
    }

    unsigned char get_type() const { return 0x02; }
};

class sequence_or_track_name : public ascii_text_meta_event
{
  public:
    sequence_or_track_name(unsigned length, const unsigned char *const p)
        : ascii_text_meta_event(length, p)
    {
    }

    unsigned char get_type() const { return 0x03; }
};

class instrument_name : public ascii_text_meta_event
{
  public:
    instrument_name(unsigned length, const unsigned char *const p)
        : ascii_text_meta_event(length, p)
    {
    }

    unsigned char get_type() const { return 0x04; }
};

class lyrics : public ascii_text_meta_event
{
  public:
    lyrics(unsigned length, const unsigned char *const p)
        : ascii_text_meta_event(length, p)
    {
    }

    unsigned char get_type() const { return 0x05; }
};

class marker : public ascii_text_meta_event
{
  public:
    marker(unsigned length, const unsigned char *const p)
        : ascii_text_meta_event(length, p)
    {
    }

    unsigned char get_type() const { return 0x06; }
};

class cue_point : public ascii_text_meta_event
{
  public:
    cue_point(unsigned length, const unsigned char *const p)
        : ascii_text_meta_event(length, p)
    {
    }

    unsigned char get_type() const { return 0x07; }
};

class midi_channel_prefix : public meta_event
{
  public:
    midi_channel_prefix(unsigned char chan) : channel(chan) {}

    unsigned char get_type() const { return 0x20; }

    unsigned get_length() const { return 1; }

  private:
    std::string get_data() const { return std::string(1, channel); }

  private:
    unsigned char channel;
};

class end_of_track : public meta_event
{
  public:
    end_of_track() {}

    std::string to_text() const
    {
        char str[260];
        sprintf(str, "(%c)DT: %5d,   meta event,   end of track",
                running ? 'r' : '*', delta_time);
        return str;
    }

    unsigned char get_type() const { return 0x2F; }

    unsigned get_length() const { return 0; }

  private:
    std::string get_data() const { return ""; }
};

class set_tempo : public meta_event
{
  public:
    set_tempo(unsigned ms_per_quarter_note)
    {
        data[0] = (ms_per_quarter_note >> 16) & 0xFF;
        data[1] = (ms_per_quarter_note >> 8) & 0xFF;
        data[2] = ms_per_quarter_note & 0xFF;
    }

    set_tempo(const unsigned char *const p)
    {
        data[0] = p[0];
        data[1] = p[1];
        data[2] = p[2];
    }

    std::string to_text()
    {
        char str[260];
        sprintf(str,
                "(%c)DT: %5d,   meta event,   type: 0x%02X(%s),   MPQN: %d",
                running ? 'r' : '*', delta_time, get_type(),
                meta_event_type_name[get_type()], get_MPQN());
        return str;
    }

    unsigned get_MPQN() const
    {
        unsigned MPQN = data[0];
        MPQN <<= 8;
        MPQN |= data[1];
        MPQN <<= 8;
        MPQN |= data[2];
        return MPQN;
    }

    unsigned char get_type() const { return 0x51; }

    unsigned get_length() const { return 3; }

  private:
    std::string get_data() const { return std::string(data, data + 3); }

  private:
    unsigned char data[3];
};

class smpte_offset : public meta_event
{
  public:
    smpte_offset(const unsigned char *const p)
    {
        data[0] = p[0];
        data[1] = p[1];
        data[2] = p[2];
        data[3] = p[3];
        data[4] = p[4];
    }

    unsigned char get_type() const { return 0x54; }

    unsigned get_length() const { return 5; }

  private:
    std::string get_data() const { return std::string(data, data + 5); }

  private:
    unsigned char data[5];
};

class time_signature : public meta_event
{
  public:
    time_signature(unsigned char n, unsigned char d, unsigned char m,
                   unsigned char _32)
        : numer(n), denom(d), metro(m), _32nds(_32)
    {
    }

    std::string to_text() const
    {
        char str[260];
        sprintf(str,
                "(%c)DT: %5d,   meta event,   type: 0x%02X(%s),  [%02X %02X "
                "%02X %02X]",
                running ? 'r' : '*', delta_time, get_type(),
                meta_event_type_name[get_type()], numer, denom, denom, _32nds);
        return str;
    }

    unsigned char get_type() const { return 0x58; }

    unsigned get_length() const { return 4; }

  private:
    std::string get_data() const
    {
        unsigned char byte[4] = {numer, denom, metro, _32nds};
        return std::string(byte, byte + 4);
    }

  private:
    unsigned char numer;
    unsigned char denom;
    unsigned char metro;
    unsigned char _32nds;
};

class key_signature : public meta_event
{
  public:
    key_signature(char k, char s) : key(k), scale(s) {}

    std::string to_text() const
    {
        char str[260];
        sprintf(str,
                "(%c)DT: %5d,   meta event,   type: 0x%02X(%s),   key: %d,   "
                "scale: %d",
                running ? 'r' : '*', delta_time, get_type(),
                meta_event_type_name[get_type()], key, scale);
        return str;
    }

    unsigned char get_type() const { return 0x59; }

    unsigned get_length() const { return 2; }

  private:
    std::string get_data() const
    {
        unsigned char byte[2] = {(unsigned char)key, (unsigned char)scale};
        return std::string(byte, byte + 2);
    }

  private:
    char key;
    char scale;
};

class sequence_specific : public meta_event
{
  public:
    sequence_specific(unsigned length, const unsigned char *const p)
        : data(std::string(p, p + length))
    {
    }

    unsigned char get_type() const { return 0x7F; }

    unsigned get_length() const { return data.size(); }

  private:
    std::string get_data() const { return data; }

  private:
    std::string data;
};
