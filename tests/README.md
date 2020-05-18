# Tests

This folder contains "tests" for OpenSupaplex. They're actually solutions for many levels recorded by users and shared
with the community.

These solutions were collected through the years by [Elmer Productions](http://www.elmerproductions.com/), and the files
in this folder were downloaded from their website: http://www.elmerproductions.com/sp/solutions.html

Each solution belongs to its author.

## How do they work

OpenSupaplex, as the SpeedFix modification of the original game, can record and play demos. Using specific command line
options, these demos can be run quickly with certain parameters:
 - `-q <path_to_demo_file>` will run the demo quickly, skipping the intro and with no game speed limit.
 - `-u <path_to_demo_file>` will run the demo without any graphical interface, just the logic of the game.

With any of these parameters, the game will just output whether the demo succeded (Murphy reached the exit with after
collecting all the Infotrons) or not.

The premise here is that in all these solutions Murphy is able to reach the exit with all the Infotrons, however…

## "Failed" solutions

…some of these solutions don't seem to be solutions at all in the original game with the SpeedFix v6.3 ([the last official
release of the SpeedFix](http://www.elmerproductions.com/sp/software.html#speedfix)).

I left them in the `failures` subfolder here for completeness as I find they fail both in OpenSupaplex and the SpeedFix 
version 6.3.

OpenSupaplex takes SpeedFix v6.3 as the reference baseline in terms of behavior and aims to preserve the very same logic.
Even some bugs in the original game might be preserved, but that's something I will think on a case-by-case basis.
