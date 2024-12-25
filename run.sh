#!/bin/bash

cd src
if clang -g -fsanitize=address main.c GeomagnetismLibrary.c -o magpoint; then
    cd ..
    ./src/magpoint 30.0 -70.0 400.0 2025.0
else
    cd ..
fi