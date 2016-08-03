#!/system/bin/sh

echo "Ingenic gsensor TESTING..."

getevent -c 1 > /tmp/test_event.log
test_eventnum /tmp/test_event.log g_sensor
eventnum=$?
if [ $eventnum -eq -1 ]
then
	echo "gsensor_test failed"
fi

echo 0 > /sys/class/i2c-adapter/i2c-1/1-001e/accelerometer/enable_device
echo 1 > /sys/class/i2c-adapter/i2c-1/1-001e/accelerometer/enable_device

if [ -e /tmp/test_gsensor.log ]
then
	rm -r /tmp/test_gsensor.log
fi

getevent -c 8 /dev/input/event$eventnum > /tmp/test_gsensor.log

test_gsensor /tmp/test_gsensor.log

if [ $? -ne 0 ]
then
	echo "gsensor_test failed"
else
	echo "gsensor_test ok"
fi
