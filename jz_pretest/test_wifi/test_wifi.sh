#!/system/bin/sh

echo "Ingenic WIFI TESTING..."
set_bg_image wifitest.png
if [ ! -e /system/lib/firmware/wifi ]
then
	mkdir -p /system/lib/firmware/wifi
fi

if [ ! -e /system/lib/firmware/wifi/fw_bcm43438.bin ]
then
	cp /sbin/pretest/fw_bcm43438.bin /system/lib/firmware/wifi/fw_bcm43438.bin
fi

if [ ! -e  /system/lib/firmware/wifi/bcm43438.cal ]
then
	cp /sbin/pretest/bcm43438.cal /system/lib/firmware/wifi/bcm43438.cal
fi

netcfg_s > /tmp/test_wifi_1.log
grep "wlan0" /tmp/test_wifi_1.log > /tmp/test_wifi_2.log
if [ $? -ne 0 ]
then
	echo "wifi_test failed"
	exit
fi

grep "DOWN" /tmp/test_wifi_2.log > /dev/null
if [ $? -eq 0 ]
then
	netcfg_s wlan0 up > /dev/null
	netcfg_s > /tmp/test_wifi_1.log
	grep "wlan0" /tmp/test_wifi_1.log | grep "UP" > /dev/null
	if [ $? -eq 0 ]
	then
		echo "wifi_test ok"
		set_bg_image wifisuccess.png
		sleep 2
	else
		echo "wifi_test failed"
		set_bg_image wififail.png
		sleep 2
	fi
else
	netcfg_s wlan0 down > /dev/null
	netcfg_s > /tmp/test_wifi_1.log
	grep "wlan0" /tmp/test_wifi_1.log | grep "DOWN" > /dev/null
	if [ $? -ne 0 ]
	then
		echo "wifi_test failed"
		set_bg_image wififail.png
		sleep 2
		exit
	fi

	netcfg_s wlan0 up > /dev/null
	netcfg_s > /tmp/test_wifi_1.log
	grep "wlan0" /tmp/test_wifi_1.log | grep "UP" > /dev/null
	if [ $? -eq 0 ]
	then
		echo "wifi_test ok"
		set_bg_image wifisuccess.png
		sleep 2
	else
		echo "wifi_test failed"
		set_bg_image wififail.png
		sleep 2
	fi
fi
