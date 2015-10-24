[Unit]
Description=Run init.%H.conf
DefaultDependencies=no
After=local-fs.target
Conflicts=shutdown.target
Before=sysinit.target shutdown.target
ConditionFileNotEmpty=/etc/init.%H.conf

[Service]
Type=oneshot
ExecStart=/bin/bash -c '/etc/init.%H.conf'
m4_ifdef(`SMACK_LABEL',
SmackProcessLabel=system-plugin-common::script
)m4_dnl

[Install]
WantedBy=sysinit.target
