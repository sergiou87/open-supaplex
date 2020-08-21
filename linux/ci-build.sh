cd linux
make -j8 -f Makefile || exit
cp -R ../resources ./OpenSupaplex-linux
rm -rf ./OpenSupaplex-linux/audio-lq # we don't need low quality audio here
rm -rf ./OpenSupaplex-linux/audio-mq # we don't need medium quality audio here
cp opensupaplex ./OpenSupaplex-linux/OpenSupaplex

zip -r OpenSupaplex-linux.zip ./OpenSupaplex-linux
