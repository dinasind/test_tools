#!/system/bin/sh

echo "Ingenic uv testing..."

test_uv > /tmp/test_uv.log

if [ $? -ne 0 ]
then
	echo "uv_test failed"
else
	echo "uv_test ok"
fi
