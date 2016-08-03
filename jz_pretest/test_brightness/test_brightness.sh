#!/system/bin/sh

#背光亮度测试
#变亮：控制背光亮度节点从50到250，步进是50
#变暗：控制背光亮度节点从250到50，步进是50

set_bg_image brightnesstest.png
centre_the_word "BRIGHTNESS TEST"
if [ ! -e /sys/class/backlight/pwm-backlight.0/brightness ]
then
	set_bg_image brightnessfail.png
	centre_the_word "can not find the node"
	exit 1
fi

#亮度上限设置
brightness=50
max=250
min=50
step=50
brightness_save=$(cat /sys/class/backlight/pwm-backlight.0/brightness)

#亮度变亮
while [ "$brightness" -lt "$max" ]
do
	echo $brightness > /sys/class/backlight/pwm-backlight.0/brightness
	brightness=$((brightness + step))
	sleep 0.5
done

brightness=250

#亮度变暗
while [ "$brightness" -gt "$min" ]
do
	echo $brightness > /sys/class/backlight/pwm-backlight.0/brightness
	brightness=$((brightness - step))
	sleep 0.5
done

#还原初始亮度
echo $brightness_save > /sys/class/backlight/pwm-backlight.0/brightness
centre_the_word "BACKLIGHT SUCCESS"
set_bg_image brightnesssuccess.png
exit 0
