/*
 * tips.cpp
 *
 *  Created on: 2015-2-6
 *      Author: LiZidong<zidong.li@ingenic.com>
 */

#include <cutils/log.h>
#include "param.h"
#include "tips.h"

int Tips::usage(char *name)
{
    unsigned int i = 0;

    ALOGD("Usage : %s [MODE] [TIME]...\n", name);
    ALOGD("--help      print this message for help\n");
    ALOGD("--version   print version\n");

    ALOGD("Example\n\n");
    ALOGD("PLAY_SINGLE_EFFECT or PLAY_EFFECT_SEQUENCE:\n");
    ALOGD("\t%s 0x01 0x60 0x60 0x60 0x60 0x60 0x60\n", name);
    ALOGD("PLAY_TIMED_EFFECT:\n");
    ALOGD("\t%s 0x03 0x20 0x10\n", name);
    ALOGD("PATTERN_RTP:\n");
    ALOGD("\t%s 0x0c 0x10 0x10 0x01 0x20 0x50 0x02\n", name);
    ALOGD("RTP_SEQUENCE:\n");
    ALOGD("\t%s 0x0d 0x70 0x70 0x09 0x70 0x50 0x40\n", name);
    ALOGD("STOP:\n");
    ALOGD("\t%s 0xff\n", name);
    ALOGD("AUDIOHAPTIC_ENABLE:\n");
    ALOGD("\t%s 0x06\n", name);
    ALOGD("AUDIOHAPTIC_DISABLE:\n");
    ALOGD("\t%s 0x07\n", name);
    ALOGD("REG_READ:\n");
    ALOGD("read register 0x01\n");
    ALOGD("\t%s 0x0a 0x01\n", name);
    ALOGD("REG_WRITE:\n");
    ALOGD("write value 0x02 to register 0x01\n");
    ALOGD("\t%s 0x09 0x01 0x02\n", name);
    return 0;
}

int Tips::print_version(void)
{
    ALOGD("Version : %s\n", VERSION);
    return 0;
}
