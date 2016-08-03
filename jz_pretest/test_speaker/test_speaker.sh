#!/system/bin/sh

echo "Ingenic speaker testing..."

echo 6 > /dev/snd/mixer3
test_speaker -P -p /sbin/pretest/sound.wav > /dev/null

if [ $? -ne 0 ]
then
	echo "speaker_test failed"
else
	echo "speaker_test finished"
fi
