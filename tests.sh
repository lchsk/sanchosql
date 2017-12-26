#!/usr/bin/env sh

sed -i 's/\/\/ #define MOCK_PG_CONN/#define MOCK_PG_CONN/' src/config.hpp

cd tests && cmake . && make -j && cd ..

for file in `find ./tests/ -maxdepth 1 -type f -executable -iregex ".*test_.+"`; do
	echo
	echo "Executing: $file";
	echo
	./"$file";
done;
