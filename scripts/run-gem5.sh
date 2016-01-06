#!/bin/sh

GEM5_PATH=/home/harry/Work/gem5/gem5-stable/

$GEM5_PATH/build/ARM/gem5.opt ./scripts/run-gem5.py >/dev/null 2>/dev/null &
GEM5=$?
sleep 1
( $GEM5_PATH/util/term/m5term 127.0.0.1 3456 ) | host/host

kill $GEM5
