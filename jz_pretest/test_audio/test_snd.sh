#!/system/bin/sh

#电流测试
function currenttest()
{
	#清除/tmp/test_select.log
	test_eventnum /tmp/check_touch.log _ts
	eventnum=$?
	if [ -e /tmp/test_select.log ]
	then
		rm -r /tmp/test_select.log
	fi
	#执行sndtest,使喇叭处在工作状态，get_select处理触摸信号
	set_bt_image ringcurrenttest.png
	getevent -c 5 /dev/input/event$eventnum > /tmp/test_select.log &
	select=0
	while [ true ]
	do
		./sndtest --replay sound_test.wav
		get_select /tmp/test_select.log
		select=$?
		case $select in
			1)set_bt_image ringcurrentsuccess.png
				break;;
			2)set_bt_image ringcurrentfail.png
				exit 2;;
			3)rm -r /tmp/test_select.log
				getevent -c 5 /dev/input/event$eventnum > /tmp/test_select.log &;;
			4)continue;;
			*)centre_the_word "select failed"
				exit 2;;
		esac
	done
}
#喇叭测试
centre_the_word "AUDIO TEST"
set_bg_image ringtest.png
./sndtest --replay sound_test.wav
if [ $? -eq 0 ]
then
	set_bg_image ringsuccess.png
	centre_the_word "REPLAY SUCCESS"
else
	set_bg_image ringfail.png
	centre_the_word "REPLAY FAILED"
	exit 1
fi
#检测是否电流测试
if [[ $CURRENT_TEST == 1 ]]
then
	currenttest
fi

#录音测试
set_bg_image recordtest.png
cat /dev/null > record_file
./sndtest --record record_file
if [ $? -eq 0 ]
then
	centre_the_word "RECORD SUCCESS"
	set_bg_image recordsuccess.png
else
	centre_the_word "RECORD FAILED"
	set_bg_image recordfail.png
	exit 1
fi

#播放录音
# replay record,The record file support 8000 & 16000 Hz
set_bg_image replay_record.png
./sndtest --speed 8000 --replay record_file
if [ $? -eq 0 ]
then
	set_bg_image replayrecordsuccess.png
	centre_the_word "REPLAY RECORD SUCCESS"
else
	set_bg_image replayrecordfail.png
	centre_the_word "REPLAY RECORD FAILED"
	exit 1
fi

rm record_file
set_bg_image black.png
