#!/bin/sh
#
# Copyright 2010
# Read the file COPYING
#
# Authors: Yeongil Jang
#
# Copyright Samsung Electronics
#

# print help message
do_help()
{
    /bin/cat >&2 <<EOF
change-booting-mode: usage:
    -?/--help        this message
    --fota           reboot for fota update (Firmware update On The Air)
    --fus            reboot for fus update (Firmware Update Service)
    --update         change system state for developer update
EOF
}

# get and check specified options
do_options()
{
	# note: default settings have already been loaded

	while [ "$#" -ne 0 ]
	do
		arg=`/usr/bin/printf %s $1 | /bin/awk -F= '{print $1}'`
		val=`/usr/bin/printf %s $1 | /bin/awk -F= '{print $2}'`
		shift
		if test -z "$val"; then
			local possibleval=$1
			/usr/bin/printf %s $1 "$possibleval" | /bin/grep ^- >/dev/null 2>&1
			if test "$?" != "0"; then
				val=$possibleval
				if [ "$#" -ge 1 ]; then
					shift
				fi
			fi
		fi

		case "$arg" in

			--fota)
				echo "Setting fota update mode" >&2
				STATUS_DIR=/opt/data/recovery
				DELTA_PATH_FILE=${STATUS_DIR}/DELTA.PATH
				if [ ! -d ${STATUS_DIR} ]; then
					echo ">> ${STATUS_DIR} does not exist. create one"
					if [ -f ${STATUS_DIR} ]; then
						/bin/rm -f ${STATUS_DIR}
					fi
					/bin/mkdir -p ${STATUS_DIR}
				fi
				echo $val > ${DELTA_PATH_FILE}
				cmd="fota"
				;;
			--fus)
				echo "Setting fus update mode" >&2
				cmd="download"
				echo "kill mtp-ui, data-router" >> /opt/var/log/fus_update.log 2>&1
				/usr/bin/killall mtp-ui >> /opt/var/log/fus_update.log 2>&1
				/usr/bin/killall data-router >> /opt/var/log/fus_update.log 2>&1
				/bin/umount /dev/gadget >> /opt/var/log/fus_update.log 2>&1
				/sbin/lsmod >> /opt/var/log/fus_update.log 2>&1
				/sbin/rmmod g_samsung >> /opt/var/log/fus_update.log 2>&1
				;;
			--update)
				echo "Setting update mode for engineers" >&2
#				echo 1 > /sys/power/noresume
#				touch /opt/etc/.hib_capturing # make fastboot image again on next booting
				/bin/mount -o remount,rw /
				if [ -f /usr/share/usr_share_locale.squash ]; then
					/bin/umount -l /usr/share/locale
					/bin/rm -rf /usr/share/locale
					/usr/bin/unsquashfs -d /usr/share/locale /usr/share/usr_share_locale.squash
					/bin/rm -rf /usr/share/usr_share_locale.squash
				fi
				exit
				;;
			-?|--help)
				do_help
				exit 0
				;;
			*)
				echo "Unknown option \"$arg\". See --help" >&2
				do_help
				exit 0
				;;
		esac
	done
}

## main

if test -z "$1"; then
	do_help
	exit 0
fi

do_options $@
/bin/sync
/sbin/reboot $cmd
