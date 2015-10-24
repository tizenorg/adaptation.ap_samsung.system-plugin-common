#  This file is copied from systemd to modify.

[Unit]
Description=Trigger Flushing of Journal to Persistent Storage
Documentation=man:systemd-journald.service(8) man:journald.conf(5)
Requires=systemd-journald.service
After=systemd-journald.service default.target

[Service]
ExecStart=/usr/bin/systemctl kill --kill-who=main --signal=SIGUSR1 systemd-journald.service
Type=oneshot
m4_ifdef(`SMACK_LABEL',
SmackProcessLabel=system-plugin-common::script
)m4_dnl
