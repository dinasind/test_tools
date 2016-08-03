#!/system/bin/sh

#蓝牙测试
#蓝牙测试方法：
# 1). Reset
# 2). Read_BD_ADDR
# 3). Set_Event_Filter
# 4). Write_Scan_Enable
# 5). Enable_Device_Under_Test_Mode

centre_the_word "BLUETOOTH TEST"
set_bg_image bluetoothtest.png

echo 0 > /sys/class/rfkill/rfkill0/state
echo 1 > /sys/class/rfkill/rfkill0/state
sleep 0.5

#从/proc/hardware/list解析出蓝牙所用串口
uart_port=$(grep "bluetooth.port" /proc/hardware/list)
uart_port="${uart_port#*:}"
uart_port="/dev/$uart_port"

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
	#执行test_bluetooth使蓝牙处在工作状态，get_select处理触摸信号
	set_bt_image bluetoothcurrenttest.png
	getevent -c 5 /dev/input/event$eventnum > /tmp/test_select.log &
	select=0
	while [ true ]
	do
		test_bluetooth  --enable_hci --baudrate 3000000 --no2bytes \
			--patchram /system/pretest/BCM4330.hcd \
			$uart_port --tosleep=50000 --use_baudrate_for_download \
			--enable_lpm #--bd_addr 43:29:b1:55:00:01 # --enable_pcm
		get_select /tmp/test_select.log
		select=$?
		case $select in
			1)set_bt_image bluetoothcurrentsuccess.png
				echo 0 > /sys/class/rfkill/rfkill0/state
				exit 0;;
			2)set_bt_image bluetoothcurrentfail.png
				echo 0 > /sys/class/rfkill/rfkill0/state
				exit 2;;
			3)rm -r /tmp/test_select.log
				getevent -c 5 /dev/input/event$eventnum > /tmp/test_select.log &;;
			4)continue;;
			*)centre_the_word "select failed"
				echo 0 > /sys/class/rfkill/rfkill0/state
				exit 2;;
		esac
	done
}

#-d为debug模式
test_bluetooth -d --enable_hci --baudrate 3000000 --no2bytes \
	--patchram /system/pretest/BCM4330.hcd \
	$uart_port --tosleep=50000 --use_baudrate_for_download \
	--enable_lpm #--bd_addr 43:29:b1:55:00:01 # --enable_pcm
if [ $? -ne 0 ]; then
	sleep 2 #等待输出结果完毕
	centre_the_word "BLUETOOTH FAILED"
	set_bg_image bluetoothfail.png
	echo 0 > /sys/class/rfkill/rfkill0/state
	exit 1
else
	sleep 2 #等待输出结果完毕
	centre_the_word "BLUETOOTH SUCCESS"
	set_bg_image bluetoothsuccess.png
	#检测是否进行电流测试
	if [[ $CURRENT_TEST == 1 ]]
	then
		currenttest
	fi
	echo 0 > /sys/class/rfkill/rfkill0/state
	exit 0
fi

