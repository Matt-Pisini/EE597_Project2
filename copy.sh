#!/bin/bash

SRC_FILE='sim2.cc'
DEST_PATH='/home/ee597/ns-3-allinone/ns-3-dev/scratch/'

cp $SRC_FILE $DEST_PATH

echo ./waf --run $SRC_FILE