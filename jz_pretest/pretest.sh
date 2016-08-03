#!/system/bin/sh

#CURRENT_TEST用来判断是否进行电流测试
CURRENT_TEST=0
#AMOLED屏幕旋转180度,TFT不支持:
#检查版本号是否为f1，为f1时旋转屏幕,f1用来给get_select判断版本
version=$(grep "ro.build.display.id" /system/build.prop)
version=${version#*=}
version=${version%_*}
version=${version%_*}
F1=0
if [ "$version" = "f1" ]
then
	CURRENT_TEST=1
	F1=1
	echo "0x36 0xc0" > /sys/devices/platform/jz-fb.0/debug/mipi_command
	echo " "
fi
export F1
export CURRENT_TEST

echo icon_error > /system/pretest/image
echo "system init ok"
echo "searching pretest scripts..."
sleep 2
export RESULT_DELAY="3.5"

#记录失败设备的数量
failed_count=0
export failed_count

#记录屏幕形状
lcd_exterior=1
export lcd_exterior

#清除checktouch文件
function clean_checktouch()
{
	if [ -e /tmp/check_touch.log ]
	then
		rm -r /tmp/check_touch.log
	fi;
}

#显示按钮的背景图片
function set_button_bg_image()
{
	picture=$1
	picture="${picture%.*}"
	echo ../../system/pretest/$picture > /system/pretest/image
	#cp /system/pretest/$1 /res/images/pretest.png
	echo -e "\n"
}

#提示点击屏幕进入下一步
function checktouch()
{
	clean_checktouch
	set_button_bg_image nexttest1.png
	getevent -c 1 > /tmp/check_touch.log
	test_eventnum /tmp/check_touch.log _ts
	eventnum=$?
	while true; do
		while [ $eventnum == -1 ];do
			clean_checktouch
			getevent -c 1 > /tmp/check_touch.log
			test_eventnum /tmp/check_touch.log _ts
			eventnum=$?
		done
		#检测是否有触摸的信息
		touch_event=$(grep "/dev/input/event$eventnum:" /tmp/check_touch.log)
		if [ "$touch_event" ]
		then
			break
		fi
		clean_checktouch
		getevent -c 1 > /tmp/check_touch.log
	done
	set_button_bg_image nexttest2.png
	sleep 0.5
	set_button_bg_image black.png
}

#匹配测试脚本
function check_scripts()
{
	mmi_item=$(grep "mmi.item" /proc/hardware/list)
	mmi_item="${mmi_item#*:}"
	mmi_item=${mmi_item//,/ }
	err=0
	for item in $mmi_item;do
		if [ "$item" == "$1" ];then
			$2
			err="$?"
			if [ "$err" -ne "0" ]; then
				echo "${item}:FAILED"
				failed_count=$((failed_count + 1))
			fi
			checktouch
		fi
	done
	return $err;
}

#测试失败设备的数组
set 'devices_list:'

#检测屏幕形状
lcd_exterior=$(grep "lcd.exterior" /proc/hardware/list )
lcd_exterior="${lcd_exterior#*exterior:}"

cd /system/pretest/
#test_erase_bootflag.sh使手表测试过程中断电后重启后，不会再进入测试模式
#test_erase_bootflag.sh
#检测测试脚本,匹配/proc/hardware/list中有的设备
checktouch
check_scripts bluetooth test_bluetooth.sh
if [ "$?" -ne "0" ];then
	set -- "$@" BLUETOOTH:FAILED
fi

#check_scripts wifi test_wifi.sh
#if [ "$?" -ne "0" ];then
#	set -- "$@" WIFI:FAILED
#fi

check_scripts charge test_charge.sh
if [ "$?" -ne "0" ];then
	set -- "$@" CHARGE:FAILED
fi

check_scripts touchkey test_touchscreen.sh
if [ "$?" -ne "0" ];then
	set -- "$@" TP:FAILED
fi

#check_scripts touchkey test_keyboard.sh
#if [ "$?" -ne "0" ];then
#	set -- "$@" KEYBOARD:FAILED
#fi


check_scripts backlight test_brightness.sh
if [ "$?" -ne "0" ];then
	set -- "$@" BACKLIGHT:FAILED
fi

check_scripts vibrator test_vibrator.sh
if [ "$?" -ne "0" ];then
	set -- "$@" VIBRATOR:FAILED
fi

check_scripts lcddisplay test_lcd.sh
if [ "$?" -ne "0" ];then
	set -- "$@" LCD:FAILED
fi

#check_scripts audioplay test_snd.sh
#if [ "$?" -ne "0" ];then
#	set -- "$@" AUDIO:FAILED
#fi

check_scripts heartrate test_heartrate.sh
if [ "$?" -ne "0" ];then
	set -- "$@" HEARTRATE:FAILED
fi

check_scripts gsensor test_frizz_gsensor.sh
if [ "$?" -ne "0" ];then
	set -- "$@" GSENSOR:FAILED
fi

check_scripts gyrsensor test_frizz_gyro.sh
if [ "$?" -ne "0" ];then
	set -- "$@" GYRO:FAILED
fi

check_scripts magsensor test_frizz_magnet.sh
if [ "$?" -ne "0" ];then
	set -- "$@" MAGNET:FAILED
fi

check_scripts proximity test_proximity.sh
if [ "$?" -ne "0" ];then
	set -- "$@" PROXIMITY:FAILED
fi

#for test in *.sh; do
#	echo -e "\n\n\n\n\n\n\n\n\n\n\n\n\n"
#	[ "$test" ==  "pretest.sh" ] && continue
#	$test
#	if [ $? -ne 0 ]; then
#		echo "${test%.sh}:"
#		echo "             failed."
#		testinfo=${test%.sh}
#		set -- "$@" ${testinfo#*_}:failed
#	fi
#done

#版本号
#version=$(getprop ro.build.display.id)
#echo "VERSION:$version"
version=$(grep "ro.build.display.id" /system/build.prop)
version=${version#*=}
echo "VERSION:$version"

#蓝牙
#echo "MAC: $(cat /data/misc/bluetooth/bt_addr)"

#输出测试失败的设备
for i in "$@"; do
    echo "$i"
done

#若有设备失败，则卡死，如不需要卡死请注释掉
if [ "$failed_count" -gt 0 ];then
	while true;do sleep 1;done
fi

#检测到触摸后结束测试
clean_checktouch
set_bg_image endtest1.png
getevent -c 1 > /tmp/check_touch.log
test_eventnum /tmp/check_touch.log _ts
eventnum=$?
while true; do
	while [ $eventnum == -1 ];do
		clean_checktouch
		getevent -c 1 > /tmp/check_touch.log
		test_eventnum /tmp/check_touch.log _ts
		eventnum=$?
	done
	#检测是否有触摸的信息
	touch_event=$(grep "/dev/input/event$eventnum:" /tmp/check_touch.log)
	if [ "$touch_event" ]
	then
		break
	fi
	clean_checktouch
	getevent -c 1 > /tmp/check_touch.log
done
set_bg_image endtest2.png
sleep 0.5
set_bg_image black.png


echo -e "pretest pass,\nreboot..."
sleep 2
