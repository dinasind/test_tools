#!/system/bin/sh

recovery erase_boot_flag

if [ $? -ne 0 ]
then
	recovery erase_boot_flag
	if [ $? -ne 0 ]
	then
		echo "erase boot flag failed"
	else
		echo "erase boot flag success"
	fi
else
	echo "erase boot flag success"
fi
