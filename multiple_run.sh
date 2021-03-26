#!/bin/bash
# INPUTS TO SCRIPT
# 1) number of trials to average over

RUN_SCRIPTA='bash run.sh A node'

for ((i=0; i<$1; i++))
do
    echo $RUN_SCRIPTA $i
done