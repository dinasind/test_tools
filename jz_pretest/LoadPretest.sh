#!/system/bin/sh

#remount system，使/system/pretest/可见，并更改所有脚本权限
echo 200 > /sys/class/backlight/pwm-backlight.0/brightness
mount -t ext4 /dev/block/platform/jzmmc_v1.2.0/by-name/system /system/

sleep 1
cd /system/pretest/
chmod 777 ./*

#开始测试
./pretest.sh
