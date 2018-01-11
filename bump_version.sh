#!/usr/bin/env sh

CURRENT_VERSION="Build 1"

echo "New version $1"

FILES=('CMakeLists.txt' 'src/main_window.hpp' 'bump_version.sh')

for file in "${FILES[@]}"; do
	echo "Bumping version in $file";
	sed -i "s/$CURRENT_VERSION/$1/" $file
done;

echo "Script finished"
