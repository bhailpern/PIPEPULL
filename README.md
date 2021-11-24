# PIPEPULL
Code for simulating pipe pulls in mario kart tour

The program is simplistic - there is no user input, if you want to change parameters
you edit the program and change the constants at the top of the file
The program can handle any size pipe, any number of high-ends, any number of rubies.
Currently, it does not distinguish betwee normals and supers - they are just not high-ends.
Currently, there is no provision for spotlilghts.

The program can be compiled with "gcc" with no special switches

To get full traces of all pipe resets and pulls, change the line
  #define DEBUG_STATUS false
to
  #define DEBUG_STATUS true
But be prepared for a LOT of trace/debug output
