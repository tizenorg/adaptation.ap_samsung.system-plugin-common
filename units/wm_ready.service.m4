[Unit]
Description=waiting for window mananger
After=xorg.service
Before=tizen-boot.target

[Service]
Type=oneshot
ExecStart=/bin/sh -c 'while [ ! -e /tmp/.wm_ready ]; do sleep 0.1 ; done'
TimeoutSec=10s
m4_ifdef(`SMACK_LABEL',
SmackProcessLabel=system-plugin-common::script
)m4_dnl

[Install]
WantedBy=tizen-boot.target
