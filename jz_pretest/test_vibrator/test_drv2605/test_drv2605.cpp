/*
 * test_drv2605.cpp
 *
 * Check whether the motor driverIC drv2605 is working properly
 *
 *  Created on: 2015-2-6
 *      Author: LiZidong<zidong.li@ingenic.com>
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>

#include "motor.h"
#include "tips.h"
#include "param.h"

/*
 * Open the device and execute vibrate.
 * Debug message is output to log file.
 */
int main(int argc, char **argv)
{
    class Motors m;
    int fd = -1;

    fd = m.motor_open();
    if (fd < 0) {
        return -1;
    }

    m.motor_exec(argc, argv);

    if (!(fd < 0))
        close(fd);

    return 0;
}
