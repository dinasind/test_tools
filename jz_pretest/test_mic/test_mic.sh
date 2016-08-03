#!/system/bin/sh

echo "Ingenic mic testing..."

test_mic > /tmp/test_mic.log

if [ $? -ne 0 ]
then
	echo "mic_test failed"
else
	echo "mic_test ok"
fi
