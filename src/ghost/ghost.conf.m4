[Ghost]
m4_ifdef(`WITH_ENGMODE',
`#output=<folder name, defaults to /var/log/ghost>
boottime=y
plotchart=y
bootchart=y',
`#output=<folder name, defaults to /var/log/ghost>
#boottime=y
#plotchart=n
#bootchart=n')
