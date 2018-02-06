del spfix.obj
del spfix.exe
tasm /m2 spfix.asm spfix.obj spfix.lst

tlink spfix.obj
