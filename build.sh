#!/bin/bash
scons "$@"
#./build/mutex_init
#./build/alarm_main
#./build/alarm_mutex
./build/cond_static
