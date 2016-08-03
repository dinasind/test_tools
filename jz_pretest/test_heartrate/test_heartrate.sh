#!/system/bin/sh

#检查心率的型号，确定测试的脚本

#检查心率型号是否为njl5303r，检查节点是否存在：
nodenum=0
while (( $nodenum < 3 ))
do
	if [ -e /sys/devices/platform/jz-adc/jz-hwmon.$nodenum/config/enable_adc ]
	then
		test_njl5303r.sh
		exit $?
	fi
	nodenum=$(( nodenum + 1 ))
done

#检查心率型号是否为pah8001，检查节点是否存在：
if [ -e /dev/pixart_ofn ]
then
	test_pah8001.sh
	exit $?
fi

#若上面两个都不是则返回失败
centre_the_word "HEARTRATE FAILED"
set_bg_image heartratefail.png
sleep 3
exit 1
