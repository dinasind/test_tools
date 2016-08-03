#!/system/bin/sh

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
	#执行test_frizz使gyro处在工作状态，get_select处理触摸信号
	set_bt_image gyrocurrenttest.png
	getevent -c 5 /dev/input/event$eventnum > /tmp/test_select.log &
	select=0
	while [ true ]
	do
		test_frizz --gyro
		get_select /tmp/test_select.log
		select=$?
		case $select in
			1)set_bt_image gyrocurrentsuccess.png
				exit 0;;
			2)set_bt_image gyrocurrentfail.png
			exit 2;;
			3)rm -r /tmp/test_select.log
				getevent -c 5 /dev/input/event$eventnum > /tmp/test_select.log &;;
			4)continue;;
			*)centre_the_word "select failed"
				exit 2;;
		esac
	done
}

#通过frizz测试gyro
centre_the_word "GYRO TEST"
set_bg_image gyrotest.png
test_frizz --gyro
if [ "$?" -ne 0 ];then
	centre_the_word "GYRO FAILED"
	set_bg_image gyrofail.png
	exit 1
else
	centre_the_word "GYRO SUCCESS"
	set_bg_image gyrosuccess.png
	#检测是否电流测试
	if [[ $CURRENT_TEST == 1 ]]
	then
		currenttest
	fi
	exit 0
fi

