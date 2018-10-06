# cpp-midi

[![Build Status](https://travis-ci.org/lgarithm/cpp-midi.svg?branch=master)](https://travis-ci.org/lgarithm/cpp-midi)

A midi tool in cpp I written in 2011.

## The Music Definition Language (MDL)

The music definition language is a simple formal language for defining MIDI music.

The MDL is designed in a way that it is easy to parse.
Every symbol is just an ascii character.

There are two kinds of symbols, the **note symbol** and **note modifier**.

There are 8 **note symbols**, including 1,2,3,4,5,6,7,s.

There are two kinds of **note modifies**, the **pitch modifiers** and the **interval modifiers**.
One note modifier can be either local or global.

A **local modifier** is a modifier that modifies the **next note symbol**.

A **global modifier** is a modifier that modifies **all note symbols after it**.

Table of note modifiers:

| &nbsp;                  | local | global |
|-------------------------|-------|--------|
| +1/12 octave            | #     | ?      |
| -1/12 octave            | b     | !      |
| +1    octave            | +     | >      |
| -1    octave            | -     | <      |
| x2 interval             | ^     | ]      |
| /2 interval             | _     | [      |
| x(2 - (1/2)^n) interval | .     |        |
