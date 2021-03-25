#!/bin/bash

SRC_FILE='sim2.cc'
DEST_PATH='/home/ee597/ns-3-allinone/ns-3-dev/scratch/'
DEST_RUN='/home/ee597/ns-3-allinone/ns-3-dev/'
COMMAND="./waf --run $SRC_FILE"
cp $SRC_FILE $DEST_PATH
cd DEST_RUN
eval COMMAND