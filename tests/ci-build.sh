make -j8 -f Makefile || exit
cp -R ../resources ./OpenSupaplex-tests
cp opensupaplex ./OpenSupaplex-tests/OpenSupaplex
cp debug-base-tests ./OpenSupaplex-tests/debug-base-tests
cp menu-throttle-tests ./OpenSupaplex-tests/menu-throttle-tests
