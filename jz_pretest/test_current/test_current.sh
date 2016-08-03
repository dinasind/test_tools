#!/system/bin/sh


#电流测试

./test_current
if [ "$?" -ne 0 ]
then
	centre_the_word "CURRENT FAILED"
	exit 1
else
	centre_the_word "CURRENT SUCCESS"
	exit 0
fi

set_bg_image black.png
