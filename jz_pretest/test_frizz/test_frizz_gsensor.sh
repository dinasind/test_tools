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
	#执行test_frizz使gsensor处在工作状态，get_select处理触摸信号
	set_bt_image gsensorcurrenttest.png
	getevent -c 5 /dev/input/event$eventnum > /tmp/test_select.log &
	select=0
	while [ true ]
	do
		test_frizz --gsensor
		get_select /tmp/test_select.log
		select=$?
		case $select in
			1)set_bt_image gsensorcurrentsuccess.png
				exit 0;;
			2)set_bt_image gsensorcurrentfail.png
				exit 2;;
			3)rm -r /tmp/test_select.log
				getevent -c 5 /dev/input/event$eventnum > /tmp/test_select.log &;;
			4)continue;;
			*)centre_the_word "select failed"
				exit 2;;
		esac
	done
}
#通过frizz测试gsensor
centre_the_word "GSENSOR TEST"
set_bg_image gsensortest.png
test_frizz --gsensor
if [ "$?" -ne 0 ];then
	centre_the_word "GSENSOR FAILED"
	set_bg_image gsensorfail.png
	exit 1
else
	centre_the_word "GSENSOR SUCCESS"
	set_bg_image gsensorsuccess.png
	#检测是否电流测试
	if [[ $CURRENT_TEST == 1 ]]
	then
		currenttest
	fi
	exit 0
fi

