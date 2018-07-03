

echo Starting > testout.txt
time >> testout.txt
cd build
SUPAPLEX.EXE @ :..\SOLVE\JENS0001.SP >> ..\testout.txt
SUPAPLEX.EXE @ :..\SOLVE\JENS0018.SP >> ..\testout.txt
SUPAPLEX.EXE @ :..\SOLVE\JENS0027.SP >> ..\testout.txt
SUPAPLEX.EXE @ :..\SOLVE\JENS0067.SP >> ..\testout.txt
SUPAPLEX.EXE @ :..\SOLVE\JENS0111.SP >> ..\testout.txt
cd ..
echo DONE >> testout.txt
time >> testout.txt


