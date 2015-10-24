[Unit]
Description=Remount rootfs
After=-.mount
Wants=-.mount
ConditionKernelCommandLine=ro
DefaultDependencies=no

[Service]
Type=oneshot
ExecStart=/bin/bash -c "/usr/bin/change-booting-mode.sh --update"
ExecStartPost=/bin/touch /run/.root_rw_complete
m4_ifdef(`SMACK_LABEL',
SmackProcessLabel=system-plugin-common::script
)m4_dnl

[Install]
WantedBy=local-fs.target
