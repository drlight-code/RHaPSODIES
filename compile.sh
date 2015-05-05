#!/bin/bash

BUILD=$1

cd RHaPSODIES/build-$BUILD ; make || exit
cd ../../RHaPSODaemon/build-$BUILD ; make || exit
cd ../../RHaPSODemo/build-$BUILD ; make || exit
