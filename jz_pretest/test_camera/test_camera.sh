#!/system/bin/sh

echo "Ingenic camera testing..."

if [ -e "/dev/video0" ]
then
	echo "camera_test ok"
else
	echo "camera_test failed"
fi
