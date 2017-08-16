#/bin/bash/

#控制http_server启停的控制脚本

ROOT_PATH=$(pwd)
BIN=${ROOT_PATH}/bin/http_server
CONF=${ROOT_PATH}/conf/httpd.conf
BIN_NAME=`basename $BIN`

ctl=$(basename $0)
function Usage()
{
	printf "Usage: %s start(-s | -S) | stop (-t | -T) | restart(-r | -R)\n" "${ctl}"
}

function Start()
{
	pid=`pidof $BIN_NAME`
	if [ $? -eq 0 ]; then
		printf "start failed! http is already running, pid is $pid.\n"
		return
	fi
	ip=$(grep -E 'IP:' $CONF | awk -F: '{print $2}')
	port=$(grep -E 'PORT:' $CONF | awk -F: '{print $2}')
	cnt=$(grep -E '^CNT' $CONF | awk -F: '{print $2}')
	proccnt=$(grep -E '^PROCCNT' $CONF | awk -F: '{print $2}')
	$BIN $ip $port $cnt $proccnt
	pid=`pidof $BIN_NAME`
	printf "start success! pid is $pid\n"
}

function Stop()
{
	pid=`pidof $BIN_NAME`
	if [ $? -ne 0 ]; then
		printf "stop failed! No httpd is running.\n"
		return
	fi
	
	killall $BIN_NAME
	printf "stop success! httpd is stoped. pid is $pid\n"
}

#检查命令行参数
if [ $# -ne 1 ] ; then
	Usage
	exit 1
fi


case $1 in
	start | -s | -S)
		Start
	;;
	stop | -t | -T)
		Stop
	;;
	restart | -r | -R)
		Stop
		Start
	;;
	*)
		Usage
		exit 2
	;;
esac
