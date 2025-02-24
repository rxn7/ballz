#!/usr/bin/env bash

script_dir=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
pushd ${script_dir}/build/release >/dev/null

if ! command -v valgrind &> /dev/null ; then 
	echo "Valgrind is not installed!"
	exit
fi

if ! command -v gprof2dot &> /dev/null ; then 
	echo "gprof2dot is not installed!"
	exit
fi

if ! command -v dot &> /dev/null ; then 
	echo "dot is not installed!"
	exit
fi

valgrind --tool=callgrind --callgrind-out-file=callgrind-result.out ./ballz
kcachegrind callgrind-result.out

rm ./callgrind-result.out

popd >/dev/null

