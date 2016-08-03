#!/system/bin/sh

#通过frizz测试magnet
centre_the_word "MAGNET TEST"
set_bg_image magnettest.png
test_frizz --magnet
if [ "$?" -ne 0 ];then
	centre_the_word "MAGNET FAILED"
	set_bg_image magnetfail.png
	exit 1
else
	centre_the_word "MAGNET SUCCESS"
	set_bg_image magnetsuccess.png
	exit 0
fi
