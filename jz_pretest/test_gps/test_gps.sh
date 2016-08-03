#!/system/bin/sh

echo "Ingenic gps testing..."

test_gps > /tmp/test_gps.log

if [ $? -ne 0 ]
then
	echo "gps_test failed"
else
	echo "gps_test ok"
fi
