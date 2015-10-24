[Unit]
Description=Check Mount
ConditionPathIsMountPoint=!/opt/usr
After=tizen-boot.target starter.service
Requires=tizen-boot.target
Before=tizen-system.target

[Service]
Type=oneshot
ExecStart=/usr/bin/check-mount.sh
ExecStartPost=/bin/sleep 3
m4_ifdef(`SMACK_LABEL',
SmackProcessLabel=system-plugin-common::script
)m4_dnl

[Install]
WantedBy=tizen-system.target
