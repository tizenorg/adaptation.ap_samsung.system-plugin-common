#!/bin/sh

BE_VERBOSE="false"
CAN_CLEANALL="false"
CLEANUP_DIR="/var/log"
THRESHOLD=90
WHITE_LIST='! -name 'messages' ! -name 'dlog_main' ! -name 'dlog_system' ! -name 'dlog_radio' ! -name 'Xorg.0.log' ! -name 'system*''

function show_help {
    echo "
clean up directory.

usage: ${0##*/} [OPTION...]
  -h, --help          show help
  -v, --verbose       provide more detailed output
  -f, --force         forced clean all if clean failed
  -d, --directory     clean up directory (default: /var/log)
  -t, --threshold     clean up threshold (default: 80%)
"
}

# Execute getopt
ARGS=$(getopt -o hvfd:t: -l "help,verbose,force,directory:threshold:" -n "$0" -- "$@");

eval set -- "$ARGS";

while true; do
    case "$1" in
        -h|--help)
            show_help >&2
            exit 0
            ;;
        -v|--verbose)
            BE_VERBOSE="true"
            shift
            ;;
        -f|--force)
            CAN_CLEANALL="true"
            shift
            ;;
        -d|--directory)
            CLEANUP_DIR=$2
            shift 2
            ;;
        -t|--threshold)
            THRESHOLD=$2
            shift 2
            re='^[0-9]+$'
            if ! [[ $THRESHOLD =~ $re ]] ; then
                echo "error: threshold value($THRESHOLD) not a number" >&2
                exit 1
            fi
            ;;
        --)
            shift;
            break;
            ;;
    esac
done

if [ $# -ne 0 ]; then
    show_help >&2
    exit 1
fi

function get_status {
    DF_RESULT=$(/bin/df $CLEANUP_DIR)
    PERCENT=$(IFS=; echo $DF_RESULT | /bin/awk 'NR==2 {print $5}')
    PERCENT_N=${PERCENT%\%}
    MOUNT_POINT=$(IFS=; echo $DF_RESULT | /bin/awk 'NR==2 {print $6}')

    if [ "z$BE_VERBOSE" == "ztrue" ]; then
        echo "+++ Status of $CLEANUP_DIR +++"
        echo "Use%: $PERCENT, Mounted at:$MOUNT_POINT"
    fi
}

function clean_up {
        get_status
        if [ $PERCENT_N -gt $THRESHOLD ];then
            if [ "z$BE_VERBOSE" == "ztrue" ]; then
                echo "Do clean up"
            fi
            /usr/bin/find $CLEANUP_DIR -type f $WHITE_LIST -exec /bin/rm -f {} \;
        fi
    return 0
}

clean_up
if [ $? -ne 0 ];then
    echo "clean up failed"
fi
