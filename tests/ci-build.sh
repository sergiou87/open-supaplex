make -j8 -f Makefile || exit
cp -R ../resources ./OpenSupaplex-tests
rm -rf ./OpenSupaplex-tests/audio-lq # we don't need low quality audio here
rm -rf ./OpenSupaplex-tests/audio-mq # we don't need medium quality audio here
cp opensupaplex ./OpenSupaplex-tests/OpenSupaplex
