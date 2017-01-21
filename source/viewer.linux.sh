#!/bin/sh

PATH=/usr/local/jdk/bin:$PATH

cd "`dirname $0`/../viewer/application.linux"
./viewer >viewer.log 2>&1
