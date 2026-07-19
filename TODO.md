# TODO

* PixelMath is doing something wrong - 15-30s on simple operations (needs to be fixed) and image names should take precedence
* Test platesolving
* Test OSC workflow

## Core

* Implement color calibration -- Something manual and something star based (needs platesolve + data!)
* Proper drizzle implementation instead of the current interpolation hack
* Implement a "Curves" tool to compliment the GHS / Stretching

 ## UI
* There is an occasional segfault on program exit. Need to trace that down.
* Double check all elements are being serialized and restored properly
* Program open "jumps" from default to serialized -- smooth it out
