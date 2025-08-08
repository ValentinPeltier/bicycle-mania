#!/bin/bash

SHADERS_DIR=shaders
OUTPUT_DIR=bin/linux-x86_64-debug

mkdir -p $OUTPUT_DIR/$SHADERS_DIR

for file in $(find $SHADERS_DIR -name \*.vert -o -name \*.frag); do
    glslc $file -o $OUTPUT_DIR/$file.spv
done