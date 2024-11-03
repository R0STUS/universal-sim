#!/usr/bin/env sh

# Move to the app directory
cd "$(dirname "$(readlink -f "$0")")" || exit

# Compile the app
exec g++ src/main.cpp -o universe-sim --std=c++20 -lsfml-graphics -lsfml-window -lsfml-system "$@"