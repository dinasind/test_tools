#!/system/bin/sh

#马达测试
#支持线性马达和普通马达两种
drv2605_path="/dev/drv2605"

centre_the_word "VIBRATOR TEST"
set_bg_image vibratortest.png

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
	#使马达处在工作状态，get_select处理触摸信号
	set_bt_image vibratorcurrenttest.png
	getevent -c 5 /dev/input/event$eventnum > /tmp/test_select.log &
	sync
	select=0
	while [ true ]
	do
		echo  300 > /sys/class/timed_output/vibrator/enable
		get_select /tmp/test_select.log
		select=$?
		case $select in
			1)set_bt_image vibratorcurrentsuccess.png
				exit 0;;
			2)set_bt_image vibratorcurrentfail.png
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

if [ -e "$drv2605_path" ];
then
    #线性马达：
	echo "drv2605 exist"
	test_drv2605 0x01 0x60 0x60 0x60
else
    #普通马达震动5秒：
	echo  5000 > /sys/class/timed_output/vibrator/enable
fi
if [ $? -ne 0 ]
then
	sleep 2 #等待震动结束
	centre_the_word "VIBRATOR FAIL"
	set_bg_image vibratorfail.png
	exit 1
else
	i=0
	while [[ $i < 5 ]]
	do
		sleep 1
		i=$(( i + 1 ))
	done
	centre_the_word "VIBRATOR SUCCESSS"
	set_bg_image vibratorsuccess.png
	#检测是否电流测试
	if [[ $CURRENT_TEST == 1 ]]
	then
		currenttest
	fi
	exit 0
fi


