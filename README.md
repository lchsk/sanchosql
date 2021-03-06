# SanchoSQL - Postgres desktop client for Linux

See [features](https://lchsk.com/sanchosql/features.html)

[SanchoSQL website](https://lchsk.com/sanchosql)

![SanchoSQL code editor](./sanchosql_editor_1.png)

## Downloads

[`deb` package (Ubuntu, Debian) and zipped archives available](https://github.com/lchsk/sanchosql/releases/tag/v0.1)

## Development

[![Build Status](https://travis-ci.org/lchsk/sanchosql.svg?branch=master)](https://travis-ci.org/lchsk/sanchosql)

## Dependencies

- gtkmm >= 3.18

- gtksourceviewmm-3.0

- libpqxx >= 4.0

### To build

- C++14 compiler

- cmake

- xmllint

On Ubuntu: `sudo apt install libpqxx-dev libgtkmm-3.0-dev libgtksourceviewmm-3.0-dev libxml2-utils`

```
mkdir build
cd build
cmake ..
make -j
./sanchosql
```


