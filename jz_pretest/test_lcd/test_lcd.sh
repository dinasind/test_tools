#!/system/bin/sh


#lcd测试,输出红绿蓝黑白满屏，红绿蓝分屏，黑白色格子满屏
centre_the_word "LCD TEST"
sleep 1
test_lcd > /tmp/test_lcd.log

if [ $? -ne 0 ]
then
	centre_the_word "LCD FAILED"
	sleep 1
	exit 1
else
	centre_the_word "LCD SUCCESS"
	sleep 1
	exit 0
fi
