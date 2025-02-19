#!/bin/sh

pushd build/debug >/dev/null
gdb ./ballz -q
popd >/dev/null
