#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <linux/soundcard.h>
#include <errno.h>

#include "sndtest.h"
#include "param.h"

/* open DEBUG macro when debugging*/
//#define DEBUG

#if defined(DEBUG)
#define pr_dbg(fmt, args...)	printf("%s(): " fmt, __func__, ##args)
#else
#define	pr_dbg(fmt, args...)
#endif

static struct run_params params;
int alarm_flag = 1;

int main(int argc, char **argv)
{
    int ret = 0;

    init_params(&params);

    ret = check_arg(argc, argv, &params);
    if (ret != 0) {
        printf("check_arg() failed !!\n");
        exit(1);
    }

    pr_dbg_params();

    ret = init_audio(&params);
    if (ret != 0) {
        printf("init_audio() failed !!\n");
        exit(1);
    }

    ret = run(&params);
    if (ret != 0) {
        printf("run() failed !!\n");
        exit(1);
    }

    close_audio(&params);
    return 0;
}

/*
 * init_params()
 *
 * Initialize the parameters to default data
 */
int init_params(struct run_params *param)
{
    param->buffer_size = DEFAULT_BUF_SIZE;
    param->channel = DEFAULT_SND_CHANNEL;
    param->format = DEFAULT_FORMAT;
    param->speed = DEFAULT_SND_SPEED;
    param->volume = DEFAULT_SND_VOLUME;
    param->replay_enable = DEFAULT_REPLAY_DISABLE;
    param->record_enable = DEFAULT_RECORD_DISABLE;
    param->second = DEFAULT_TIME;
    param->replay_fd = -1;
    param->record_fd = -1;
    param->dsp_fd = -1;
    param->mixer_fd = -1;
    return 0;
}

/*
 * check_arg()
 *
 * check the transfer parameters & set options such as format, channel...
 */
int check_arg(int argc, char **argv, struct run_params *param)
{
    char *arg = NULL;
    char *val = NULL;
    int format = -1;
    int channel = -1;
    int rate = -1;
    int mode = -1;
    int volume = -1;
    int speed = -1;
    int buffer_size = -1;
    int second = -1;

    if ((argc < 2)) {
        return usage();
    } else if (argc == 2 && !strcmp(argv[1], "--version")) {
        return version();
    } else if (argc == 2 && !strcmp(argv[1], "--help")) {
        return usage();
    }

    argc--;
    argv++;

    while (argc > 0) {
        arg = argv[0];
        val = argv[1];
        if (argc < 2) {
            printf("Error : argument number is not even\n");
            return usage();
        }
        argc -= 2;
        argv += 2;
        if (!strcmp(arg, "--format")) {
            format = set_format(val, param);
        } else if (!strcmp(arg, "--channel")) {
            channel = atoi(val);
        } else if (!strcmp(arg, "--rate")) {
            rate = atoi(val);
        } else if (!strcmp(arg, "--volume")) {
            volume = atoi(val);
        } else if (!strcmp(arg, "--speed")) {
            speed = atoi(val);
        } else if (!strcmp(arg, "--buffer")) {
            buffer_size = atoi(val);
        } else if (!strcmp(arg, "--time")) {
            second = atoi(val);
        } else if (!strcmp(arg, "--replay")) {
            param->replay_enable = 1;
            mode = RECORD;
            strcpy(param->replay_file, val);
        } else if (!strcmp(arg, "--record")) {
            param->record_enable = 1;
            mode = REPLAY;
            strcpy(param->record_file, val);
        } else {
            printf("Error : Wrong parameter [%s]\n", arg);
            return -1;
        }
    }

    if (param->replay_enable && param->record_enable) {
        printf("Error : Can not record & replay at the same time.\n");
        return -1;
    } else if (!(param->replay_enable || param->record_enable)) {
        printf("Error : You should chose record or replay mode.\n");
        return -1;
    }

    if (format >= 0)
        param->format = format;
    if (channel >= 0)
        param->channel = channel;
    if (rate >= 0)
        param->rate = rate;
    if (volume >= 0)
        param->volume = volume;
    if (speed >= 0)
        param->speed = speed;
    if (buffer_size >= 0)
        param->buffer_size = buffer_size;
    if (second > 0)
        param->second = second;

    return 0;
}

/*
 * usage()
 */
int usage(void)
{
    printf("Usage : ./sndtest [OPTION]...[FILE]\n\n");

    printf("--help          print this message\n");
    printf("--version       print version\n");
    printf("--format          chose which format\n");
    printf("--channel       chose which channel\n");
    printf("--record        chose record mode & file\n");
    printf("--replay        chose replay mode & file\n");
    printf("--volume        chose volume(0~100)\n");

    printf("\n\nRecognized sample formats are:");
    printf(" U8, S16_LE, S24_LE");
    printf("\n");

    return -1;
}

int version(void)
{
    printf("This is sndtest version%s\n", VERSION);
    return -1;
}

int pr_dbg_params(void)
{
    pr_dbg("buffer_size        %d\n", params.buffer_size);
    pr_dbg("format             %d\n", params.format);
    pr_dbg("channel            %d\n", params.channel);
    pr_dbg("speed              %d\n", params.speed);
    pr_dbg("file_type          %d\n", params.file_type);
    pr_dbg("replay_enable      %d\n", params.replay_enable);
    pr_dbg("record_enable      %d\n", params.record_enable);
    pr_dbg("rate               %d\n", params.rate);
    pr_dbg("volume             %d\n", params.volume);
    pr_dbg("time               %ds\n", params.second);

    if (params.replay_enable) {
        pr_dbg("file               %s\n", params.replay_file);
    }
    if (params.record_enable) {
        pr_dbg("file               %s\n", params.record_file);
    }

    return 0;
}

int set_format(char *format, struct run_params *param)
{
    unsigned int i = 0;
    int fmt = 0;

    for (i = 0; i < sizeof(supported_fmts) / sizeof(supported_fmts[0]); i++) {
        if (!strcmp(supported_fmts[i], format))
            break;
    }

    if (i == sizeof(supported_fmts) / sizeof(supported_fmts[0]))
        return -1;

    switch (i) {
    case SND_FMT_U8:
    case SND_FMT_S8:
        fmt = FMT8BITS;
        break;

    case SND_FMT_U16_BE:
    case SND_FMT_U16_LE:
        fmt = FMT16BITS;
        break;

    case SND_FMT_S16_BE:
    case SND_FMT_S16_LE:
    default:
        fmt = FMT16BITS;
        break;
    }

    return fmt;
}

/*
 * init_audio()
 *
 * Open the devices & initialize audio configurations
 */
int init_audio(struct run_params *param)
{
    struct stat f_stat;

    if (param->replay_enable && param->replay_file[0] != '\0') {
        param->dsp_fd = open_device(DEVICE_DSP_NAME, O_WRONLY);
        if (param->dsp_fd < 0) {
            printf("%s: Cannot open %s\n", __func__, DEVICE_DSP_NAME);
            return -1;
        }

        param->replay_fd = open_file(param->replay_file, O_RDONLY);
        if (param->replay_fd < 0) {
            printf("%s: Cannot open %s\n", __func__, param->replay_file);
            return -1;
        }
        if (stat(param->replay_file, &f_stat) == -1)
            printf("Cannot get file size\n");
        else
            param->replay_file_length = f_stat.st_size;
    }

    if (param->record_enable && param->record_file[0] != '\0') {
        param->dsp_fd = open_device(DEVICE_DSP_NAME, O_RDONLY);
        if (param->dsp_fd < 0) {
            printf("%s: Cannot open %s\n", __func__, DEVICE_DSP_NAME);
            return -1;
        }

        param->jz_dmic_fd = open_device(DEVICE_JZ_DMIC, O_RDONLY);
        if (param->jz_dmic_fd < 0) {
            printf("%s: Cannot open %s\n", __func__, DEVICE_JZ_DMIC);
            return -1;
        }

        param->record_fd = open_file(param->record_file, O_WRONLY);
        if (param->record_fd < 0) {
            printf("%s: Cannot open %s\n", __func__, param->record_file);
            return -1;
        }
        if (stat(param->record_file, &f_stat) == -1)
            printf("Cannot get file size\n");
        else
            param->record_file_length = f_stat.st_size;
    }

    param->mixer_fd = open_device(DEVICE_MIXER_NAME, O_WRONLY);
    if (param->mixer_fd < 0) {
        printf("%s: Cannot open %s\n", __func__, DEVICE_MIXER_NAME);
        return -1;
    }

    config_device(param);
    pr_dbg("Finish init_audio()\n");

    return 0;
}

int open_device(char *file, int flag)
{
    int fd = -1;
    struct stat stats;

    if ((lstat(file, &stats) == 0) && S_ISREG(stats.st_mode)) {
        pr_dbg("Warning: device file is a regulator file.\n");
    }

    fd = open(file, flag);
    if (fd < 0)
        printf("Cannot open device %s, errno:%s\n", file, strerror(errno));

    return fd;
}

int open_file(char *file, int flag)
{
    int fd = -1;

    fd = open(file, flag);
    if (fd < 0)
        printf("Cannot open file %s\n", file);

    return fd;
}

/*
 * config_device()
 *
 * Initialize audio configurations
 */
int config_device(struct run_params *param)
{
    int ioctl_val = 0;

    pr_dbg("SNDCTL_DSP_CHANNELS   %u\n", SNDCTL_DSP_CHANNELS);
    pr_dbg("SNDCTL_DSP_SETFMT   %u\n", SNDCTL_DSP_SETFMT);
    pr_dbg("SNDCTL_DSP_SPEED   %u\n", SNDCTL_DSP_SPEED);
    pr_dbg("SOUND_MIXER_WRITE_VOLUME   %u\n", SOUND_MIXER_WRITE_VOLUME);
    pr_dbg("SOUND_MIXER_READ_VOLUME   %u\n", SOUND_MIXER_READ_VOLUME);

    ioctl_val = param->channel;
    if (ioctl(param->dsp_fd, SNDCTL_DSP_CHANNELS, &ioctl_val) < 0) {
        printf("%s: set channels error: %s\n", __func__, strerror(errno));
    }
    if (ioctl_val != param->channel) {
        printf("%d channel is not supported,"
                "changed to %d by driver\n", param->channel, ioctl_val);
    }

    ioctl_val = param->format;
    if (ioctl(param->dsp_fd, SNDCTL_DSP_SETFMT, &ioctl_val) < 0) {
        printf("%s: set format error: %s\n", __func__, strerror(errno));
    }
    if (ioctl_val != param->format) {
        printf("%d format is not supported,"
                "changed to 0x%08x by driver\n", param->format, ioctl_val);
    }

    ioctl_val = param->speed;
    if (ioctl(param->dsp_fd, SNDCTL_DSP_SPEED, &ioctl_val) < 0) {
        printf("%s: set speed error: %s\n", __func__, strerror(errno));
    }
    if (ioctl_val != param->speed) {
        printf("%d speed is not supported,"
                "changed to %d by driver\n", param->speed, ioctl_val);
    }

    if (param->replay_enable) {
        ioctl_val = param->volume;
        pr_dbg("vol =     %d\n", ioctl_val);
        if (ioctl(param->mixer_fd, SOUND_MIXER_WRITE_VOLUME, &ioctl_val) < 0) {
            printf("%s: set volume error: %s\n", __func__, strerror(errno));
        }
        if (ioctl_val != param->volume) {
            printf("%d volume is not supported,"
                    "changed to %d by driver-----\n", param->volume, ioctl_val);
        }
    }

#if 0
    if (param->record_enable) {
        if (ioctl(param->mixer_fd, SOUND_MIXER_READ_VOLUME, &ioctl_val) < 0) {
            printf("%s: set volume error: %s\n", __func__, strerror(errno));
        }
        if (ioctl_val != param->volume) {
            printf("%d volume is not supported,"
                    "changed to %d by driver\n", param->volume, ioctl_val);
        }
    }
#endif

    return 0;
}

int close_audio(struct run_params *param)
{
    if (param->replay_enable || param->record_enable)
        close(param->dsp_fd);
    if (param->replay_enable)
        close(param->replay_fd);
    if (param->record_enable) {
        close(param->record_fd);
        close(param->jz_dmic_fd);
    }

    close(param->mixer_fd);
    pr_dbg("close audio file\n");

    return 0;
}

void sig_alram(int sig)
{
    pr_dbg("alarm!\n");
    alarm_flag = 0;
    return;
}

/*
 * run()
 *
 * The main running function.
 * Replay & record, and use alarm() to realize the function of timing
 */
int run(struct run_params *param)
{
    int len = 0;
    int num = 0;
    char *buff = NULL;
    char buf_byte;
    int ops = 0;
    int second = 0;

    buff = malloc(DEFAULT_BUF_SIZE);
    if (buff == NULL) {
        printf("Cannot malloc enough memory\n");
        return -1;
    }

    signal(SIGALRM, sig_alram);
    alarm(param->second);
    if (param->record_enable) {
        buf_byte = '8';
        write(param->mixer_fd, &buf_byte, 1);
        pr_dbg("Recording...\n");
        while (alarm_flag) {
            // Record node file change from DEVICE_DSP_NAME("/dev/snd/dsp")
            // to DEVICE_JZ_DMIC("/dev/jz-dmic")
            len = read(param->jz_dmic_fd, buff, DEFAULT_BUF_SIZE);
            if (len < 0) {
                printf("Read sound device error\n");
                break;
            }
            if (len == 0) {
                printf("Read zero bytes!\n");
                continue;
            }

            num = len;

            len = write(param->record_fd, buff, num);
            if (len < 0) {
                printf("Save sound data to file failed\n");
                break;
            }
        }
    }

    if (param->replay_enable) {
        buf_byte = '6';
        write(param->mixer_fd, &buf_byte, 1);
        pr_dbg("Replaying...\n");
        while (ops < param->replay_file_length && alarm_flag) {
            len = read(param->replay_fd, buff, DEFAULT_BUF_SIZE);
            if (len < 0) {
                printf("Read music error\n");
                break;
            }
            if (len == 0) {
                printf("Read music zero bytes!\n");
                continue;
            }

            ops += len;
            num = len;

            len = write(param->dsp_fd, buff, num);
            if (len < 0) {
                printf("Write music failed\n");
                break;
            }
        }
    }

    free(buff);
    pr_dbg("Finish run()\n");
    return 0;
}
