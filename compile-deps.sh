#!/bin/sh

for dir in ../VistaCoreLibs ; do
	DIR=$(pwd)

	cd $dir/build-debug
	make $* || exit
	cd $DIR
done
