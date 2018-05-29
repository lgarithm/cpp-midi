#pragma once

void reverse_byte_order(unsigned &x)
{
    x = (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
}

void reverse_byte_order(unsigned short &x) { x = (x << 8) | (x >> 8); }

unsigned get_reversed_value(unsigned x)
{
    return (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
}
