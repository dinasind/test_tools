#ifndef PARAM_H_
#define PARAM_H_

/* device names */
#define DEVICE_DSP_NAME         "/dev/snd/dsp"
#define DEVICE_MIXER_NAME       "/dev/snd/mixer"
#define DEVICE_JZ_DMIC          "/dev/jz-dmic"

/* default parameters */
#define DEFAULT_BUF_SIZE        1024 * 2
#define DEFAULT_SND_CHANNEL     2
#define DEFAULT_FORMAT          FMT16BITS
#define DEFAULT_SND_SPEED       44100
#define DEFAULT_SND_VOLUME      80
#define DEFAULT_REPLAY_DISABLE  0
#define DEFAULT_RECORD_DISABLE  0
#define DEFAULT_TIME            5

#define FMT8BITS    AFMT_U8
#define FMT16BITS   AFMT_S16_LE
//#define FMT24BITS   AFMT_S24_LE

#define SND_FMT_S8          0
#define SND_FMT_U8          1
#define SND_FMT_S16_LE      2
#define SND_FMT_S16_BE      3
#define SND_FMT_U16_LE      4
#define SND_FMT_U16_BE      5
#define SND_FMT_S24_3LE     6
#define SND_FMT_S24_3BE     7
#define SND_FMT_U24_3LE     8
#define SND_FMT_U24_3BE     9

enum run_mode {
    RECORD = 1,
    REPLAY = 2
};

//  The speed will restrict by drivers, so user can select speed by themselves
/*
int support_speed[] = {
    8000,
    11025,
    12000,
    16000,
    22050,
    24000,
    32000,
    44100,
    48000,
    96000,
};
*/
const char *supported_fmts[] = {
    "S8",
    "U8",
    "S16_LE",
    "S16_BE",
    "U16_LE",
    "U16_BE",
    "S24_3LE",
    "S24_3BE",
    "U24_3LE",
    "U24_3BE",
};

#endif /* PARAM_H_ */
