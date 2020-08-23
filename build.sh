#!/bin/bash
scons "$@"
./build/mutex_init
./build/alarm_main
