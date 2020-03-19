# Supaplex
[Supaplex](https://en.wikipedia.org/wiki/Supaplex) is a game made in the early nineties.

![image](https://user-images.githubusercontent.com/3305301/42215866-25134eb4-7ec0-11e8-913d-b64604632fa8.png)

Since then the source code has been lost. Some enthusiasts have disassembled the game,
fixed many bugs, and added many features. Today the game and the so-called speed fix can
be downloaded free from [here](http://www.elmerproductions.com/sp/dlinst.html). 

The source code for the speed fix is likewise, unavailable.

# What this is
This project  is an effort to disassemble and reimplement the game in C.

This project is based on [cilliemalan's work](https://github.com/cilliemalan/supaplex), and therefore here you'll find assembly files,
and an IDA database that contains the dissasembled and analyzed source of the SPFIX63.EXE file.

But it also includes the new implementation using C and SDL2.

## Getting Started

If you want to build the original game from its disassembled source code, please refer to
the instructions in [cilliemalan's repo](https://github.com/cilliemalan/supaplex).

### Prerequisites
- You need SDL2 installed (on macOS: `brew install sdl2`)
- Some toolchain to build the C code (on macOS: just having Xcode is enough)
- You will need the [original Supaplex files](https://cdn.chills.co.za/supaplex.zip).
    Extract the files into the `build` directory.

### Building
Right now only an Xcode project is provided along with the C source code. Just opening
that project in macOS and building it will be enough.

More generic building support via Makefile will be added in the future, but you should
be able to build the game easily in any platform.


# License
As expressed in [cilliemalan's repo](https://github.com/cilliemalan/supaplex), the
disassembled assembly source code of the game is released under the MIT license.

On the other hand, I release the reimplementation in C under the GPLv3 license.

If you have a problem with this please don't hesitate to contact me.
