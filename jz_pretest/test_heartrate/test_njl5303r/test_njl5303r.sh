#!/system/bin/sh

#心率测试，测试前请确定心率型号
centre_the_word "HEARTRATE TEST"
set_bg_image heartratetest.png
#检查节点是否存在
nodenum=0
node=1
while (( $nodenum < 3 ))
do
	if [ -e /sys/devices/platform/jz-adc/jz-hwmon.$nodenum/config/enable_adc ]
	then
		node=/sys/devices/platform/jz-adc/jz-hwmon.$nodenum/config/enable_adc
		break
	fi
	nodenum=$(( nodenum + 1 ))
done
#检查是否找到节点，未找到节点则返回失败
if [[ $node == 1 ]]
then
	centre_the_word "can not find the node"
	set_bg_image heartratefail.png
	exit 1
fi

echo 0 > $node
count=0
#控制led闪烁
while (( $count < 300 ));do
	echo 1 > $node
	sleep 0.03
	count=$((count + 1))
	echo 0 > $node
	sleep 0.03
	count=$((count + 1))
done

if [[ $? == 0 ]]
then
	centre_the_word "HEARTRATE SUCCESS"
	set_bg_image heartratesuccess.png
	exit 0
else
	centre_the_word "HEARTRATE FAILED"
	set_bg_image heartratefail.png
	exit 1
fi
