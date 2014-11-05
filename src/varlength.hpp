#ifndef __VARLENGTH_HPP__
#define __VARLENGTH_HPP__

#include <vector>

unsigned char* write_variable_length(unsigned value, unsigned char *&p)
{
   register unsigned buffer;
   buffer = value & 0x7F;

   while ( value >>= 7 )
   {
     buffer <<= 8;
     buffer |= ((value & 0x7F) | 0x80);
   }

   while (true)
   {
	  *p++ = buffer & 0xFF;

      if (buffer & 0x80)
          buffer >>= 8;
      else
          break;
   }

   return p;
}

void write_variable_length(unsigned value, std::string &buffer)
{
	register unsigned tmp = value & 0x7F;
	while (value >>= 7)
	{
		tmp <<= 8;
		tmp |= ((value & 0x7F) | 0x80);
	}

	while (1)
	{
		buffer.push_back((unsigned char) (tmp & 0xFF));
		if (tmp & 0x80) tmp >>= 8;
		else break;
	}
}

unsigned read_variable_length(const unsigned char *&p)
{
	register unsigned value;
	register unsigned char c;

    if ((value = *p++) & 0x80)
    {
		value &= 0x7F;
		do
		{
			value = (value << 7) + ((c = *p++) & 0x7F);
		} while (c & 0x80);
    }

    return value;
}


#endif
