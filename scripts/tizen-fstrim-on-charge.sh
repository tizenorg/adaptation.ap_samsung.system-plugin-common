#!/bin/sh

if [ "$#" -ne 1 ];then
	echo "Argument was missed."
	exit 1
fi

CHARGE_NOW_FILE=`/usr/bin/find /sys/devices -path */power_supply/battery/charge_now`
if [ "x$CHARGE_NOW_FILE" == "x" ]; then
	echo "Can not find 'charge_now'."
	exit 1
else
	CHARGE_NOW_VALUE=`/bin/cat $CHARGE_NOW_FILE`
fi

if [ "$CHARGE_NOW_VALUE" -gt 0 ];then
	echo "Do fstrim."
	/sbin/fstrim $*
else
	echo "Not on charging."
fi
