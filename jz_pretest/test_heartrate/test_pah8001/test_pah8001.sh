#!/system/bin/sh

centre_the_word "HEARTRATE TEST"
set_bg_image heartratetest.png

# 测试项--test=1,需要保证闪光灯在5.5cm之内无遮挡,
# 用于测试PAH8001在SMT贴片是否良好
test_pah8001 --test=1 --time=5
if [ "$?" -ne 0 ];then
	sleep 4   #等待结果输出
	pah8001_stop 2>>err.txt 1>>err.txt
	exit 1
else
	centre_the_word "HEARTRATE SUCCESS"
	set_bg_image heartratesuccess.png
	sleep 1
	exit 0
fi

# 测试项--test=2,需要保证18%灰板和闪光灯之间的距离在15mm左右
# 用于测试闪光灯是否贴歪了，是否有遮光，透光等问题
#test_pah8001 --test=2 --time=5
#if [ "$?" -ne 0 ];then
#	sleep 4
#	centre_the_word "HEARTRATE FAILED"
#	set_bg_image heartratefail.png
#	sleep 4
#	exit 1
#else
#	centre_the_word "HEARTRATE SUCCESS"
#	set_bg_image heartratesuccess.png
#	sleep 1
#	exit 0
#fi
