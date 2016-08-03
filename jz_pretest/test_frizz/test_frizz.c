#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <getopt.h>

#define FRIZZ_DEV "/dev/frizz"

#define IOC_MAGIC '9'
#define MAX_HWFRIZZ_FIFO_SIZE   64      /*!< Max frizz hw fifo size (Byte)*/
#define FRIZZ_IOCTL_HARDWARE 64  /*!< Start Index of Hardware IOCTL */
/*!< fizz work status test*/
#define FRIZZ_IOCTL_FW_TEST  _IOW(IOC_MAGIC, FRIZZ_IOCTL_HARDWARE + 4, sensor_info*)

typedef struct{
    uint32_t sensor_id;
    uint32_t test_loop;
	uint32_t raw_data[MAX_HWFRIZZ_FIFO_SIZE];
} sensor_info;

static int
frizz_gsensor_test(int fd, int option)
{
	int cnt;
	int err = -1;
	int try_times = 5;
	int i;
	sensor_info test_sensor_info = {0x80, 0x01, {0x00}};

	do {
		memset(&test_sensor_info.raw_data[0], 0x00, sizeof(test_sensor_info.raw_data));
		cnt = ioctl(fd, FRIZZ_IOCTL_FW_TEST, &test_sensor_info);
		printf("cnt = %d\n", cnt);
		try_times--;
	} while (try_times > 0 && cnt != 5);

	if (test_sensor_info.raw_data[0] == 0xff808004 &&
        test_sensor_info.raw_data[1] != 0x00 &&
        test_sensor_info.raw_data[2] != 0x00 &&
        test_sensor_info.raw_data[3] != 0x00 &&
        test_sensor_info.raw_data[4] != 0x00)
	{
		err = 0;
	} else {
		err = -ENODEV;
	}

	for(i = 0; i < cnt; i++){
		printf("0x%08x\n", test_sensor_info.raw_data[i]);
		fflush(stdout);
	}

	return err;
}

static int
frizz_gyro_test(int fd, int option)
{
	int cnt;
	int err = -1;
	int try_times = 5;
	int i;
	sensor_info test_sensor_info = {0x82, 0x01, {0x00}};

	do {
		memset(&test_sensor_info.raw_data[0], 0x00, sizeof(test_sensor_info.raw_data));
		cnt = ioctl(fd, FRIZZ_IOCTL_FW_TEST, &test_sensor_info);
		printf("cnt = %d\n", cnt);
		try_times--;
	} while (try_times > 0 && cnt != 6);

	if (test_sensor_info.raw_data[0] == 0xff808205 &&
        test_sensor_info.raw_data[1] != 0x00 &&
        test_sensor_info.raw_data[2] != 0x00 &&
        test_sensor_info.raw_data[3] != 0x00 &&
        test_sensor_info.raw_data[4] != 0x00 &&
        test_sensor_info.raw_data[5] != 0x00)
	{
		err = 0;
	} else {
		err = -ENODEV;
	}

	for(i = 0; i < cnt; i++){
		fflush(stdout);
		printf("0x%08x\n", test_sensor_info.raw_data[i]);
	}

	return err;
}

static int
frizz_magent_test(int fd, int option)
{
	int cnt;
	int err = -1;
	int try_times = 5;
	int i;
	sensor_info test_sensor_info = {0x81, 0x01, {0x00}};

	do {
		memset(&test_sensor_info.raw_data[0], 0x00, sizeof(test_sensor_info.raw_data));
		cnt = ioctl(fd, FRIZZ_IOCTL_FW_TEST, &test_sensor_info);
		printf("cnt = %d\n", cnt);
		try_times--;
	} while (try_times > 0 && cnt != 5);

	if (test_sensor_info.raw_data[0] == 0xff808104 &&
        test_sensor_info.raw_data[1] != 0x00 &&
        test_sensor_info.raw_data[2] != 0x00 &&
        test_sensor_info.raw_data[3] != 0x00 &&
        test_sensor_info.raw_data[4] != 0x00)
	{
		err = 0;
	} else {
		err = -ENODEV;
	}

	for(i = 0; i < cnt; i++){
		fflush(stdout);
		printf("0x%08x\n", test_sensor_info.raw_data[i]);
	}

	return err;
}


void usage(int argc, char *argv[])
{
	printf("Usage: %s --gsensor --gyro --magnet --help\n", argv[0]);
	printf("    --help       Display help information\n");
	printf("    --gsensor    test gsensor\n");
	printf("    --gyro       test gyro\n");
	printf("    --magnet     test magnet\n");
}

int main(int argc, char **argv)
{
	int fd = -1;
	int gsensor = 0;
	int gyro = 0;
	int magnet = 0;
	int digit_optind = 0;
	int c;
	int err = 0;

	while (1) {
		int this_option_optind = optind ? optind : 1;
		int option_index = 0;
		static struct option long_options[] = {
			{"gsensor", 0, 0, 1},
			{"gyro",    0, 0, 2},
			{"magnet",  0, 0, 3},
			{"help",    0, 0, 4},
			{0, 0, 0, 0}
		};

		c = getopt_long(argc, argv, "", long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
		case 1:
			gsensor = 1;
			break;
		case 2:
			gyro = 1;
			break;
		case 3:
			magnet = 1;
			break;
		case 4:
			usage(argc, argv);
			exit(0);
		default:
			usage(argc, argv);
			exit(-1);
		}
	}

	fd = open(FRIZZ_DEV, O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "couldn't open %s, %s\n", FRIZZ_DEV, strerror(errno));
		return -1;
	}

	if (gsensor)
		err = frizz_gsensor_test(fd, 0x00);

	if (gyro)
		err = frizz_gyro_test(fd, 0x00);

	if (magnet)
		err = frizz_magent_test(fd, 0x00);


	close(fd);
	fd = -1;
	return err;
}
