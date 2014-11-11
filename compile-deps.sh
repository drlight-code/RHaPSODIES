#!/bin/sh

for dir in ../VistaCoreLibs ../VistaFlowLib ../VflModules/VflTextRendering ../VflModules/Vfl2DDiagrams ; do
	DIR=$(pwd)

	cd $dir/build-debug
	make || exit
	cd $DIR
done
