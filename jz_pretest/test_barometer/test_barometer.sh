#!/system/bin/sh

echo "Ingenic barometer testing..."

test_barometer > /tmp/test_barometer.log

if [ $? -ne 0 ]
then
	echo "barometer_test failed"
else
	echo "barometer_test ok"
fi
