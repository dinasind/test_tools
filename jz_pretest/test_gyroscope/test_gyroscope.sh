#!/system/bin/sh

echo "Ingenic gyroscope TESTING..."

getevent -c 1 > /tmp/test_event.log
test_eventnum /tmp/test_event.log gyroscope_sensor
eventnum=$?
if [ $eventnum -eq -1 ]
then
	echo "gyroscope_test failed"
fi

echo 0 > /sys/class/i2c-adapter/i2c-2/2-0068/iio\:device0/gyro_enable
echo 1 > /sys/class/i2c-adapter/i2c-2/2-0068/iio\:device0/gyro_enable
#echo 0 > /sys/class/i2c-adapter/i2c-1/1-006a/enable_device
#echo 1 > /sys/class/i2c-adapter/i2c-1/1-006a/enable_device

if [ -e /tmp/test_gyro.log ]
then
	rm -r /tmp/test_gyro.log
fi

getevent -c 8 /dev/input/event$eventnum > /tmp/test_gyro.log

test_gyroscope /tmp/test_gyro.log

if [ $? -ne 0 ]
then
	echo "gyroscope_test failed"
else
	echo "gyroscope_test ok"
fi
