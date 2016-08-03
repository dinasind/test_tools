#!/system/bin/sh

#通过frizz测试barometric pressure sensor
centre_the_word "GSENSOR TEST"
set_bg_image bpsensortest.png
test_frizz --bpsensor
if [ "$?" -ne 0 ];then
	centre_the_word "BAROMETRIC PRESSURE SENSOR FAILED"
	set_bg_image bpsensorfail.png
	exit 1
else
	centre_the_word "BAROMETRIC PRESSURE SENSOR SUCCESS"
	set_bg_image bpsensorsuccess.png
	exit 0
fi

