#!/bin/bash

cd macos
#xcodebuild -project OpenSupaplex/OpenSupaplex.xcodeproj -scheme OpenSupaplex -configuration Release
xcodebuild -project OpenSupaplex/OpenSupaplex.xcodeproj -scheme OpenSupaplex -configuration Release -archivePath ./OpenSupaplex-macOS.xcarchive archive
pushd OpenSupaplex-macOS.xcarchive/Products/Applications
zip -r ../../../OpenSupaplex-macOS.zip OpenSupaplex.app
popd
