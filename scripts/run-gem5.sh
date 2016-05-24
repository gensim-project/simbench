#!/bin/sh

GEM5_PATH=/home/toky/work/simulators/gem5-stable/
SIMBENCH_PATH=/home/toky/work/pamela/simbench/

if [ -r $SIMBENCH_PATH/scripts/run-gem5.py ] ; then
    echo "run-gem5 script: $SIMBENCH_PATH/scripts/run-gem5.py"
else
    echo "File not found or wrong permission: $SIMBENCH_PATH/scripts/run-gem5.py"
    exit 1
fi


if [ -x $SIMBENCH_PATH/scripts/run-gem5.py ] ; then
    echo "run-gem5 script: $SIMBENCH_PATH/scripts/run-gem5.py"
else
    echo "Execution permission needed: $SIMBENCH_PATH/scripts/run-gem5.py"
    exit 1
fi

if [ -x $GEM5_PATH/build/ARM/gem5.opt ] ; then
    echo "run-gem5 script: $GEM5_PATH/build/ARM/gem5.opt"
else
    echo "File not found or wrong permission : $GEM5_PATH/build/ARM/gem5.opt"
    exit 1
fi


if [ -x $SIMBENCH_PATH/host/host ] ; then
    echo "run-gem5 script: $SIMBENCH_PATH/host/host"
else
    echo "File not found or wrong permission: $SIMBENCH_PATH/host/host"
    exit 1
fi

if [ -x $GEM5_PATH/util/term/m5term  ] ; then
    echo "run-gem5 script: $GEM5_PATH/util/term/m5term"
else
    echo "File not found or wrong permission: $GEM5_PATH/util/term/m5term  "
    exit 1
fi

if [ -r  $SIMBENCH_PATH/out/arm/realview-pba8/simbench ] ; then
    echo "run-gem5 script: $SIMBENCH_PATH/out/arm/realview-pba8/simbench"
else
    echo "File not found or wrong permission: $SIMBENCH_PATH/out/arm/realview-pba8/simbench"
    exit 1
fi






$GEM5_PATH/build/ARM/gem5.opt $SIMBENCH_PATH/scripts/run-gem5.py >/dev/null 2>/dev/null &
GEM5=$?
sleep 1
( $GEM5_PATH/util/term/m5term 127.0.0.1 3456 ) | $SIMBENCH_PATH/host/host

kill $GEM5
