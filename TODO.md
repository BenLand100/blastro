# TODO

* Test OSC workflow
* Scripting interface -- algorithms, other manipulations, need a way to chain with logic


## Core

* Double check pixel rejection when drizzle is enabled (satellite trail in lion nebula Sii data 18 frames 2.5 sigma)
* Implement color calibration -- manually, and then star based (will need SP data)
    a) manually using robust scale and median for background neutralization and then whatever is traditional for equalizing the non background bits
    b) star based using the platesolve info and starfinder to extract SP metrics to compare to SP data (from somewhere)
* Need to get really crisp on the way algorithm settings are handled, which ones exist, and descriptions for usage docs. 
    a) need to make sure it is a core drive to keep this documentation up to date with changes

 ## UI

* Algorithm Dialogs and the PCL dialogs need a concept of 'presets' that can 
    a) store the configuration parameters for the underlying algorithm/PCL process
    b) act as a quick way to recall configurations of the dialogs
    c) work with the Preprocessing Wizard sections (if possible)
    d) consists of workspace-level cache that can be persisted with the project 
    e) so that things like Preprocessing Wizard that integrate algoithms can also choose from the presets. 
    f) probably also add a presets menu item with options to save and load just the presets and configuration for algorithms/processes, separately from images.
* Undo and Redo need some attention, especially when PCL is involved
* There is an occasional segfault on program exit. Need to trace that down.
* Program open "jumps" from default to serialized -- smooth it out

## PCL Bridge

UNIMPLEMENTED PCL API CALL: Font/SetFontWeight
UNIMPLEMENTED PCL API CALL: Font/GetFontHeight
UNIMPLEMENTED PCL API CALL: Sizer/SetSizerAlignment
UNIMPLEMENTED PCL API CALL: Control/SetControlFont
UNIMPLEMENTED PCL API CALL: Font/GetFontHeight
UNIMPLEMENTED PCL API CALL: Control/SetControlFont