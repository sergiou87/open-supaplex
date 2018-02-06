

all: spfix.exe

clean:
	del spfix.obj
	del spfix.exe

spfix.obj: clean spfix.asm
	tasm /m2 spfix.asm spfix.obj spfix.lst

spfix.exe: spfix.obj
	tlink spfix.obj

