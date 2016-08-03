/*
 * motor.cpp
 *
 *  Created on: 2015-2-6
 *      Author: LiZidong<zidong.li@ingenic.com>
 */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <cutils/log.h>
#include "motor.h"
#include "tips.h"

Motors::Motors()
{
    motor.fd = -1;
    motor.param =NULL;
}

Motors::~Motors()
{
    if (motor.fd >= 0)
        close(motor.fd);
    if (motor.param != NULL)
        delete[] motor.param;
}

/*
 * Main function to execute vibrate
 */
int Motors::motor_exec(int argc, char **argv)
{
    class Tips tips;
	int i = 0;
	int size = 0;
	int ret = 0;
	char *str;
	int len = 0;

	if (argc < 2) {
	    tips.usage(argv[0]);
		return -1;
	}

	size = argc - 1;
	motor.param = new char[size];
	for (i = 0; i < argc; i++) {
		motor.param[i - 1] = (int)strtol(argv[i], &str, 16);
	}

	len = argc - 1;
	ret = write(motor.fd, motor.param, len);
	if (ret != len) {
	    ALOGD("Write have some problem\n");
		return -1;
	}

	return 0;
}

int Motors::motor_open(void)
{
	motor.fd = open(DEVICE_PATH, O_RDWR);
	if (motor.fd < 0) {
	    ALOGD("Can not open %s\n", DEVICE_PATH);
		return -1;
	}

	return motor.fd;
}
