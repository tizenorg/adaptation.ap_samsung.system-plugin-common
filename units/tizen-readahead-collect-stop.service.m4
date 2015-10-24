[Unit]
Description=Stop Read-Ahead Data Collection
DefaultDependencies=no
Conflicts=shutdown.target
After=tizen-readahead-collect.service boot-animation.service
Before=shutdown.target tizen-system.target
ConditionVirtualization=no

[Service]
Type=oneshot
ExecStart=/usr/bin/systemd-notify --readahead=done
m4_ifdef(`SMACK_LABEL',
SmackProcessLabel=system-plugin-common::script
)m4_dnl

[Install]
Also=tizen-readahead-collect.service
