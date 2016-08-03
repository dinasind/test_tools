#!/system/bin/sh

#近距离传感器测试
centre_the_word "PROXIMITY TEST"
set_bg_image proximitytest.png

sensor_name=""

if [ -e /sys/kernel/pac7673/enable_proximity ]
then
	sensor_name="pac7673"
elif [ -e /sys/kernel/em30719/enable_proximity ]
then
	sensor_name="em30719"
	echo "em30719 PROXIMITY"
else
	echo "unsupport sensor"
	exit 1
fi

node=/sys/kernel/$sensor_name/enable_proximity
enable=/sys/kernel/$sensor_name/enable_proximity

echo 1 > $enable

#先从getevent中获取到触摸屏设备的eventnum
#test_event正确用法应该是 <文件名> <设备名>，
getevent -c 1 > /tmp/test_proximity.log
test_eventnum /tmp/test_proximity.log $sensor_name
eventnum=$?
if [ $eventnum -eq -1 ]
then
	centre_the_word "PROXIMITY TEST FAILED"
	set_bg_image proximityfail.png
	exit 1
fi


if [ -e /tmp/test_proximity.log ]
then
	rm -r /tmp/test_proximity.log
fi

#输出距离的坐标值
getevent -c 4 /dev/input/event$eventnum > /tmp/test_proximity.log
get_proximity /tmp/test_proximity.log
sleep 1
grep "0003 0009" /tmp/test_proximity.log > /dev/null
if [ "$?" -ne 0 ]
then
	centre_the_word "PROXIMITY FAILED"
	set_bg_image proximityfail.png
	echo 0 > $enable
	exit 1
else
	centre_the_word "PROXIMITY SUCCESS"
	set_bg_image proximitysuccess.png
	echo 0 > $enable
	exit 0
fi

set_bg_image black.png
