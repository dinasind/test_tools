#!/system/bin/sh

echo "Ingenic mpu9250 testing..."
set_bg_image invensense.png

sysfs_path="/sys/bus/iio/devices/iio:device0"
f1="pedometer_steps"
f2="pedometer_time"
f3="pedometer_clip_threshold"
f4="pedometer_min_steps"
f5="pedometer_max_step_buffer_time"
f6="pedometer_threshold"
f7="pedometer_min_up_time"
f8="pedometer_max_up_time"
f9="pedometer_min_energy"

unexist=0

for file in $f1 $f2 $f3 $f4 $f5 $f6 $f7 $f8 $f9
do
	if [ ! -e "$sysfs_path/$file" ]
	then
		unexist=1
		break
	fi
done

if [ $unexist -eq 1 ]
then
	echo "mpu9250_test failed"
else
	echo "mpu9250_test ok"
fi
