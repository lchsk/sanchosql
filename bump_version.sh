#!/usr/bin/env bash

CURRENT_VERSION="0.0.1"

echo "New version $1"

FILES=('CMakeLists.txt' 'src/config.hpp' 'bump_version.sh')

for file in "${FILES[@]}"; do
	echo "Bumping version in $file";
	sed -i "s/$CURRENT_VERSION/$1/" $file
done;

echo "Script finished"
