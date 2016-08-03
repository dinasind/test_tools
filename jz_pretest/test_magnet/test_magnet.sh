#!/system/bin/sh

echo "Ingenic magnetometer TESTING..."

getevent -c 1 > /tmp/test_event.log
test_eventnum /tmp/test_event.log magnetic_sensor
eventnum=$?
if [ $eventnum -eq -1 ]
then
	echo "magnetometer_test failed"
fi

echo 0 > /sys/class/i2c-adapter/i2c-1/1-001e/magnetometer/enable_device
echo 1 > /sys/class/i2c-adapter/i2c-1/1-001e/magnetometer/enable_device

if [ -e /tmp/test_magnet.log ]
then
	rm -r /tmp/test_magnet.log
fi

getevent -c 8 /dev/input/event$eventnum > /tmp/test_magnet.log

test_magnet /tmp/test_magnet.log

if [ $? -ne 0 ]
then
	echo "magnetometer_test failed"
else
	echo "magnetometer_test ok"
fi
