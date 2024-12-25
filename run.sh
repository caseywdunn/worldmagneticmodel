#!/bin/bash

cd src
if clang -g -fsanitize=address main.c GeomagnetismLibrary.c -o main; then
    cd ..
    ./src/main 30.0 -70.0 400.0 2025.0
else
    cd ..
fi