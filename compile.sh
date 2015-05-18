#!/bin/bash

BUILD=$1
shift

cd RHaPSODIES/build-$BUILD ; make $@ || exit
cd ../../RHaPSODaemon/build-$BUILD ; make $@ || exit
cd ../../RHaPSODemo/build-$BUILD ; make $@ || exit
