/*
 * motor.h
 *
 *  Created on: 2015-2-6
 *      Author: LiZidong<zidong.li@ingenic.com>
 */

#ifndef MOTOR_TEST_H
#define MOTOR_TEST_H

#include "param.h"

class Motors
{
public:
    Motors();
    ~Motors();
    int motor_exec(int argc, char **argv);
    int motor_open(void);

private:
    struct motor_data motor;
};

#endif
