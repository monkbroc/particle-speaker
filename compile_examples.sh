#!/bin/bash
# Compile each library example as a test

set -xe

PLATFORMS="photon electron"
EXAMPLES=`find examples -mindepth 1 -maxdepth 1 -not -empty -type d -not -name playWavFromSD | sort`

for platform in $PLATFORMS; do
  for example in $EXAMPLES; do
    echo particle --no-update-check compile $platform $example
  done
done

