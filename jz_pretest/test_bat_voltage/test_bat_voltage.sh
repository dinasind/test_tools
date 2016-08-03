#!/system/bin/sh

#echo "Ingenic battery voltage testing..."

if [ -e /sys/class/power_supply/battery/voltage_now ]
then
	vol=`cat /sys/class/power_supply/battery/voltage_now`
		echo "battery voltage: $vol mV, bat_voltage_test ok"
else
	echo "bat_voltage_test failed"
fi
