#!/system/bin/sh

#echo "Ingenic battery voltage testing..."

if [ -e /sys/frizz/get_version ]
then
	frizz_version=`cat /sys/frizz/get_version`
	echo "Frizz version: $frizz_version, frizz_test ok"
else
	echo "frizz_test failed"
fi
sleep 2
