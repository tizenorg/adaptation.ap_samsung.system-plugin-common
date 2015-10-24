[Unit]
Description=Cleanup Storage
DefaultDependencies=no

[Service]
Type=oneshot
ExecStart=/usr/bin/cleanup-storage.sh
m4_ifdef(`SMACK_LABEL',
SmackProcessLabel=system-plugin-common::script
)m4_dnl

[Install]
WantedBy=basic.target
