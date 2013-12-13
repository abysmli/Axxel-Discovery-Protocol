#!/bin/sh
#
# chkconfig: 2345  - -
# Description: 
# 
# processname: adp.sh
#
# Source function library
. /etc/rc.status

ret=0

start() {
	# check adp status
	echo
	echo "starting Axxel Discovery Protocol service..."
	/usr/sbin/adp
	ret=$?
	if [ $ret -eq 0 ]
	then
		echo -e "Status: \033[30;32mActiv! \033[0m"
		echo 
	else
		echo -e "Start \033[30;31mFailed! \033[0m"
		echo 
	fi
}

stop() {
	echo
	echo "stopping Axxel Discovery Protocol service..."
	kill `pgrep adp`
	ret=$?
	if [ $ret -eq 0 ]
	then
		echo -e "Status: \033[30;31mClosed! \033[0m"
		echo
	else
		echo -e "Stop: \033[30;31mFailed! \033[0m"
		echo
	fi
}

status() {
	local result
	echo "checking Status of Axxel Discovery Protocol..."
	
	result=$( ps -e | grep adp | grep -v grep | grep -v adp.sh | awk '{print $1}')
	if [ -n "$result" ] 
	then
		echo
		echo -e "Status: \033[30;32mAktiv! \033[0m"
		echo
		ret=0
	else
		echo
		echo -e "Status: \033[30;31mDown! \033[0m"
		echo
		ret=1
	fi
}

case "$1" in
	start)
		start
		;;
	stop)
		stop
		;;
	status)
		status
		;;
	*)
		echo $"usage: $0 {start|stop|status}"
		exit 1
esac

exit $ret
