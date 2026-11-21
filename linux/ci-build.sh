cd linux
make -j8 -f Makefile || exit
cp -R ../resources ./OpenSupaplex-linux
cp opensupaplex ./OpenSupaplex-linux/OpenSupaplex

zip -r OpenSupaplex-linux.zip ./OpenSupaplex-linux
