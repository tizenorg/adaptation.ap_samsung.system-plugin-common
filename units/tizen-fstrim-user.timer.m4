[Unit]
Description=Timer for tizen-fstrim

[Timer]
m4_ifdef(`WITH_FREQUENT_FSTRIM',
`OnBootSec=30
OnUnitActiveSec=3h',
`OnCalendar=*-*-* 06:00:00')
