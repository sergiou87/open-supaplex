#Supaplex

[Supaplex](https://en.wikipedia.org/wiki/Supaplex) is a game made in the early nineties.

Since then the source code has been lost. Some enthusiasts have disassembled the game,
fixed many bugs, and added many features. Today the game and the so-called speed fix can
be downloaded free from [here](http://www.elmerproductions.com/sp/dlinst.html). 

The source code for the speed fix is likewise, unavailable.

# What this is

This project  is an effort to disassemble and understand the speed fix
executable once again.

Here you'll find assembly files, and IDA databases that contain the dissasembled and
analyzed source of the SPFIX63.EXE file.

## How to build

### Prerequisites
- In order to run the project the contents of `supaplex.zip`, downloaded from
    the link above needs to be extracted to the `build` directory.
- You will also need [DosBox](https://www.dosbox.com/) in your PATH.
- To build you will need DOS version of Turbo Assembler. Google a bit and you should
    be able to find it. You'll need tasm and tlink. Extract all the files into
    the `tasm` directory.

### Building
Open DOSBox and mount this directory. To do this, start DOSBox and type this:
```
mount C <wherever this is>
C:
```


# License

I could not find any license with the original executable prohibiting
disassembly and modification of the source code. As such I release the
disassembled source code of the game under the MIT license. If you have
a problem with this please don't hesitate to contact me.