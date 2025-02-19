#!/bin/sh

cmake --build build/release

if [ $? -ne 0 ]; then
	exit
fi

pushd build/release >/dev/null
./ballz
popd >/dev/null
