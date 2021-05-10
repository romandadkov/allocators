#!/bin/bash
echo "re build"
cmake -S. -Bbuild
cmake --build build
echo "re check"
build/bin/unit_tests
