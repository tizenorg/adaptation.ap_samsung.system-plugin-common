#!/bin/sh

while [ "z`/bin/grep /opt/usr /proc/mounts`" == "z" ]
do /bin/sleep 0.5
done
