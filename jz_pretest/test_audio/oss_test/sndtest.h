#ifndef SNDTEST_H_
#define SNDTEST_H_

#define VERSION     "1.0"

struct run_params {
    int dsp_fd;
    int mixer_fd;
    int replay_fd;
    int record_fd;
    int jz_dmic_fd;
    int buffer_size;
    int replay_data_len;
    int format;
    int channel;
    int speed;
    int file_type;
    int replay_enable;
    int record_enable;
    int rate;
    int volume;
    char replay_file[127];
    char record_file[127];
    int replay_file_length;
    int record_file_length;
    int second;
};

int check_arg(int argc, char **argv, struct run_params *param);
int usage(void);
int init_audio(struct run_params *param);
int open_device(char *file, int flag);
int open_file(char *file, int flag);
int config_device(struct run_params *param);
int init_params(struct run_params *param);
int run(struct run_params *param);
int close_audio(struct run_params *param);
int set_format(char *format, struct run_params *param);
int pr_dbg_params(void);
int version(void);

#endif /* SNDTEST_H_ */
