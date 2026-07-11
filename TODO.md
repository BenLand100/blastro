# TODO

* Test platesolving
* Test OSC workflow

## Core

* Speed up Background Extraction -- multithreading / openmp probably not used
* Implement color calibration -- Something manual and something star based (needs platesolve + data!)
* Proper drizzle implementation instead of the current interpolation hack
* "Other" stretches (luminance / rbg already selectable, but need saturation, maybe hue, etc.)
* Implement a "Curves" tool to compliment the GHS / Stretching

 ## UI
* There is an occasional segfault on program exit. Need to trace that down.
* Double check all elements are being serialized and restored properly
* Program open "jumps" from default to serialized -- smooth it out
