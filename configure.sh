#!/bin/sh

COMMON_FLAGS="-GNinja ."

cmake -Bbuild/debug -DCMAKE_BUILD_TYPE=Debug $COMMON_FLAGS -DCMAKE_EXPORT_COMPILE_COMMANDS=ON 
cmake -Bbuild/release -DCMAKE_BUILD_TYPE=Release $COMMON_FLAGS

# Mingw builds
if command -v x86_64-w64-mingw32-cmake &> /dev/null ; then
	echo "x86_64-w64-mingw32-cmake found. Windows build will be created"

	x86_64-w64-mingw32-cmake -Bbuild/win/debug -DCMAKE_BUILD_TYPE=Debug $COMMON_FLAGS 
	x86_64-w64-mingw32-cmake -Bbuild/win/release -DCMAKE_BUILD_TYPE=Release $COMMON_FLAGS 
else
	echo "x86_64-w64-mingw32-cmake not found. Windows builds will not be created!"
fi
