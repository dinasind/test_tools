#!/system/bin/sh

echo "write vcom voltage ..."

test_write_vcom $1 > /dev/null

if [ $? -ne 0 ]
then
	echo "write_vcom_voltage failed"
else
	echo "write_vcom_voltage ok"
fi
