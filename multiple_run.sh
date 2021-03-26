#!/bin/bash
# INPUTS TO SCRIPT
# 1) number of trials to average over

RUN_SCRIPT='bash run.sh A node'

for ((i=0; i<$1; i++))
do
    eval $RUN_SCRIPT $i
done