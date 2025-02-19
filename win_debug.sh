#!/bin/sh

cmake --build ./build/win/debug

if [ $? -ne 0 ]; then
	exit
fi

pushd build/win/debug >/dev/null
wine ./ballz.exe
popd >/dev/null
