all : clean sndtest

sndtest : sndtest.c
	mipsel-linux-android-gcc sndtest.c -o sndtest

clean :
	rm -rf sndtest *.o
