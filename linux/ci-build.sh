make -j8 -f Makefile.tests || exit
cp -R ../resources ./OpenSupaplex-linux
rm -rf ./OpenSupaplex-linux/audio-lq # we don't need low quality audio here
cp opensupaplex ./OpenSupaplex-linux/OpenSupaplex
