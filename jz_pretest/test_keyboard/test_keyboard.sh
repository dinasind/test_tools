#!/system/bin/sh

#按键测试
sleep 1
./inputevent 2>>err.txt 1>>err.txt
if [ "$?" -ne 0 ];then
	centre_the_word "KEY FAILED"
	exit 1
else
	centre_the_word "KEY SUCCESS"
	exit 0
fi

