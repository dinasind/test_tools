#!/system/bin/sh


#触摸屏测试
./test_touchbutton

if [ "$?" -ne 0 ]
then
	centre_the_word "TP FAILED"
	set_bg_image touchscreenfail.png
	exit 1
else
	centre_the_word "TP SUCCESS"
	set_bg_image touchscreensuccess.png
	exit 0
fi

set_bg_image black.png
