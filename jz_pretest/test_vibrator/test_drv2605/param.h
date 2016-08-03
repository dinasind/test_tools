/*
 * param.h
 *
 *  Created on: 2015-2-6
 *      Author: zdli
 */

#ifndef PARAM_H_
#define PARAM_H_

#define VERSION             "1.2"

#define HAPTICS_DEVICE_NAME "drv2605"
#define DEVICE_PATH         "/dev/drv2605"
/* Commands */
#define HAPTIC_CMDID_PLAY_SINGLE_EFFECT     0x01
#define HAPTIC_CMDID_PLAY_EFFECT_SEQUENCE   0x02
#define HAPTIC_CMDID_PLAY_TIMED_EFFECT      0x03
#define HAPTIC_CMDID_GET_DEV_ID             0x04
#define HAPTIC_CMDID_RUN_DIAG               0x05
#define HAPTIC_CMDID_AUDIOHAPTIC_ENABLE     0x06
#define HAPTIC_CMDID_AUDIOHAPTIC_DISABLE    0x07
#define HAPTIC_CMDID_AUDIOHAPTIC_GETSTATUS  0x08

#define HAPTIC_CMDID_REG_WRITE              0x09
#define HAPTIC_CMDID_REG_READ               0x0a
#define HAPTIC_CMDID_REG_SETBIT             0x0b

#define HAPTIC_CMDID_PATTERN_RTP            0x0c
#define HAPTIC_CMDID_RTP_SEQUENCE           0x0d
#define HAPTIC_CMDID_GET_EFFECT_COUNT       0x10
#define HAPTIC_CMDID_UPDATE_FIRMWARE        0x11
#define HAPTIC_CMDID_READ_FIRMWARE          0x12
#define HAPTIC_CMDID_STOP                   0xFF

struct motor_data {
    int fd;
    char *param;
};

#endif /* PARAM_H_ */
