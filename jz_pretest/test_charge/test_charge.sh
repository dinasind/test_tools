#!/system/bin/sh

#USB充电测试
#插或拔超过1次结束测试

centre_the_word "CHARGE TEST"

node=/sys/class/power_supply/acpwr/online
if [ ! -e $node ]
then
	set_bg_image chargefail.png
	centre_the_word "CAN'T FIND NODE"
	exit 1
fi

count=0
time=0
laststate=`cat $node`

#检测当前状态
if [ "$laststate" -eq 1 ]
then
	set_bg_image charge_poweron.png
else
	set_bg_image charge_poweroff.png
fi

#电流测试
function currenttest()
{
	#清除/tmp/test_select.log
	test_eventnum /tmp/check_touch.log _ts
	eventnum=$?
	if [ -e /tmp/test_select.log ]
	then
		rm -r /tmp/test_select.log
	fi
	#这里没有操作，直到收到结束信号就进行下一步。get_select处理触摸信号
	set_bt_image chargecurrenttest.png
	getevent -c 5 /dev/input/event$eventnum > /tmp/test_select.log &
	select=0
	sleep 0.2
	while [ true ]
	do
		get_select /tmp/test_select.log
		select=$?
		case $select in
			1)set_bt_image chargecurrentsuccess.png
				exit 0;;
			2)set_bt_image chargecurrentfail.png
				exit 2;;
			3)rm -r /tmp/test_select.log
				getevent -c 5 /dev/input/event$eventnum > /tmp/test_select.log &;;
			4)continue;;
			*)centre_the_word "select failed"
				exit 2;;
		esac
		sleep 0.2
	done
}


#重复插拔电源，超过60秒自动结束，并返回失败
while [ time -lt 120 ]
do
	state=`cat $node`

    #未充电
	if [ "$state" -eq 0 ]
	then
		if [ "$laststate" -eq 1 ]
		then
			set_bg_image charge_poweroff.png
			count=$((count + 1))
		fi
		laststate=0
	fi

	#充电中
	if [ "$state" -eq 1 ]
	then
		if [ "$laststate" -eq 0 ]
		then
			set_bg_image charge_poweron.png
			count=$((count + 1))
		fi
		laststate=1
	fi

	#错误状态
	if [ "$state" -ne 0 ] && [ "$state" -ne 1 ]
	then
		set_bg_image chargefail.png
		centre_the_word "READ CHARGE STATE FAILED"
		exit 1
	fi

	#更改插拔次数
	if [ "$count" -gt 0 ]
	then
		centre_the_word "CHARGE SUCCESS"
		set_bg_image chargesuccess.png
		#检测是否电流测试
		if [[ $CURRENT_TEST == 1 ]]
		then
			currenttest
		fi
		exit 0
	fi
	time=$((time + 1))
	sleep 0.5
done
if [ time -gt 119 ]
then
	exit 1
fi
