

all: spfix.exe

clean:
	del spfix.obj
	del spfix.exe

spfix.obj: clean spfix63u.asm
	tasm /m2 spfix63u.asm spfix.obj spfix.lst

spfix.exe: spfix.obj
	tlink spfix.obj

