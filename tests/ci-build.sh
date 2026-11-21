ruby test-runner-tests.rb || exit
make -j8 -f Makefile || exit
cp -R ../resources ./OpenSupaplex-tests
cp opensupaplex ./OpenSupaplex-tests/OpenSupaplex
cp debug-base-tests ./OpenSupaplex-tests/debug-base-tests
cp menu-throttle-tests ./OpenSupaplex-tests/menu-throttle-tests
cp ranking-tests ./OpenSupaplex-tests/ranking-tests
cp hall-of-fame-tests ./OpenSupaplex-tests/hall-of-fame-tests
