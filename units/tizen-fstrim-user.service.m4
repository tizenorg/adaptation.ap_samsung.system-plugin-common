[Unit]
Description=Discard unused blocks on user partition
Requires=opt-usr.mount

[Service]
Type=oneshot
ExecStart=/usr/bin/tizen-fstrim-on-charge.sh /opt/usr
StandardOutput=journal
StandardError=inherit
m4_ifdef(`SMACK_LABEL',
SmackProcessLabel=system-plugin-common::script
)m4_dnl
