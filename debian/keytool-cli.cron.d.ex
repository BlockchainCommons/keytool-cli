#
# Regular cron jobs for the keytool-cli package.
#
0 4	* * *	root	[ -x /usr/bin/keytool-cli_maintenance ] && /usr/bin/keytool-cli_maintenance
