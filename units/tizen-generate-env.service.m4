[Unit]
Description=Generate environment from /etc/profile.d
DefaultDependencies=no
After=opt.mount
Before=basic.target

[Service]
Type=oneshot
ExecStart=/usr/bin/env -i sh -c 'source /etc/profile; env | /bin/egrep -v "^(HOME|PWD|SHLVL|_)=" > /run/tizen-mobile-env'
ExecStartPost=/usr/bin/chsmack -a "_" /run/tizen-mobile-env
m4_ifdef(`SMACK_LABEL',
SmackProcessLabel=system-plugin-common::script
)m4_dnl

[Install]
WantedBy=basic.target
