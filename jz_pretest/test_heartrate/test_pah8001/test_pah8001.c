/*
 *
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <time.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <errno.h>

#define PAH8001_DEV "/dev/pixart_ofn"
#define PIXART_IOC_MAGIC 'h'
#define PIXART_IOCTL_ENABLE_DISABLE (0)
#define PIXART_IOCTL_FACTORY_TEST_START (1)
#define PIXART_IOCTL_FACTORY_TEST_GET (2)
#define PIXART_IOCTL_FACTORY_TEST_END (3)
#define PIXART_IOC_MAXNR 4

#define PIXART_IOCTL_SENSOR_ENABLE_DISABLE \
            _IOW(PIXART_IOC_MAGIC, PIXART_IOCTL_ENABLE_DISABLE, int)
#define PIXART_IOCTL_SENSOR_FACTORY_TEST_START \
            _IOW(PIXART_IOC_MAGIC, PIXART_IOCTL_FACTORY_TEST_START, int)
#define PIXART_IOCTL_SENSOR_FACTORY_TEST_GET \
            _IOW(PIXART_IOC_MAGIC, PIXART_IOCTL_FACTORY_TEST_GET, int)
#define PIXART_IOCTL_SENSOR_FACTORY_TEST_END \
            _IOW(PIXART_IOC_MAGIC, PIXART_IOCTL_FACTORY_TEST_END, int)

#define FACTORY_TESTING_CHECK_MECHANICAL_ASSEMBLY 0x01
#define FACTORY_TESTING_CHECK_FRAME_AVERAGE       0x02
#define DISABLE                                   0x00
#define ENABLE                                    0x01

typedef enum {
	BANK0 = 0,
	BANK1,
} bank_e;

struct pah8001_reg {
	unsigned char bank; /* BANK0 or BANK1 */
    unsigned char reg_addr;
	unsigned char reg_val;
};

int poweron(int fd)
{
	int en = ENABLE;
	int err = 0;

	err = ioctl(fd, PIXART_IOCTL_SENSOR_ENABLE_DISABLE, &en);
	usleep(400 * 1000);

	return err;
}

int poweroff(int fd)
{
	int en = DISABLE;
	int err = 0;

	err = ioctl(fd, PIXART_IOCTL_SENSOR_ENABLE_DISABLE, &en);

	return err;
}

/*
 * 测试项--test=1,需要保证闪光灯无遮挡
 * 用于测试PAH8001在SMT贴片是否良好
 * @fd: value from open()
 * @timeout: how many seconds for test
 */
static int pah8001_factory_testing_check_mechanical_assembly(int fd, int timeout)
{
	int err = -1;
	int val = FACTORY_TESTING_CHECK_MECHANICAL_ASSEMBLY;
	int count = 0;

	struct pah8001_reg reg;

	timeout *= 10;
    /* request for Factory Testing_Check Mechanical Assembly */
	err = ioctl(fd, PIXART_IOCTL_SENSOR_FACTORY_TEST_START, &val);
	if (err) {
		fprintf(stderr, "couldn't check mechanical assembly, %s\n", strerror(errno));
		return err;
	}

	/* get value of register 0x0B */
	reg.bank = BANK0;
	reg.reg_addr = 0x08;
	do {
		err = ioctl(fd, PIXART_IOCTL_SENSOR_FACTORY_TEST_GET, &reg);
		if (err) {
			fprintf(stderr, "could not get the value of register 0x%x, %s\n", reg.reg_addr, strerror(errno));
			return err;
		}
		printf("reg[0x%x][2:0]=%d\n", reg.reg_addr, reg.reg_val & 0x7);
		fflush(stdout);
		usleep(100 * 1000);
		if ((reg.reg_val & 0x07) == 6)
			count++;
	} while (timeout-- > 0);

	err = ioctl(fd, PIXART_IOCTL_SENSOR_FACTORY_TEST_END, 0);
	if (err) {
		fprintf(stderr, "couldn't top factory test, %s\n", strerror(errno));
		return err;
	}
	/*Read Bank0 Addr 0x08[2:0], the value must be 6
	 *检查0x08寄存器值是否为06，若为06则count++，若count超过5次则认为成功
	 */
	return (count > 5 ? 0 : -EFAULT);
}

/*
 * 测试项--test=2,需要保证18%灰板和闪光灯之间的距离在15mm左右
 * 用于测试闪光灯是否贴歪了，是否有遮光，透光等问题
 * @fd: value from open()
 * @timeout: how many seconds for test
 */
static int pah8001_factory_testing_check_frame_average(int fd, int timeout)
{
	int err = -1;
	int val = FACTORY_TESTING_CHECK_FRAME_AVERAGE;
	struct pah8001_reg reg;

	timeout *= 10;
    /* request for Factory Testing_Check Frame Average */
	err = ioctl(fd, PIXART_IOCTL_SENSOR_FACTORY_TEST_START, &val);
	if (err) {
		fprintf(stderr, "couldn't check frame average, %s\n", strerror(errno));
		return err;
	}

	/* get value of register 0x1B */
	reg.bank = BANK1;
	reg.reg_addr = 0x1B;
	do {
		err = ioctl(fd, PIXART_IOCTL_SENSOR_FACTORY_TEST_GET, &reg);
		if (err) {
			fprintf(stderr, "could not get the value of register 0x%x, %s\n", reg.reg_addr, strerror(errno));
			break;
		}
		printf("reg[0x%x]=%d\n", reg.reg_addr, reg.reg_val);
		fflush(stdout);
		usleep(100 * 1000);
	} while (timeout-- > 0);

	err = ioctl(fd, PIXART_IOCTL_SENSOR_FACTORY_TEST_END, 0);
	if (err) {
		fprintf(stderr, "couldn't top factory test, %s\n", strerror(errno));
		return err;
	}

	return 0;
}

void usage(int argc, char *argv[])
{
	printf("Usage: %s --test=1,2 --time=nsec\n", argv[0]);
	printf("    --help       Display help information\n");
	printf("    --test=1,2   1 means Factory Testing_Check Mechanical Assembly\n");
	printf("                 2 means Factory Testing_Check Frame Average\n");
	printf("    --time=nsec  nsec means how many senconds the test consumed\n");
}

int main(int argc, char *argv[])
{
	int fd = -1;
	int time = -1;
	int test = -1;
	int digit_optind = 0;
	int c;
	int err = -1;

	while (1) {
		int this_option_optind = optind ? optind : 1;
		int option_index = 0;
		static struct option long_options[] = {
			{"test", 1, 0, 1},
			{"time", 1, 0, 2},
			{"help", 0, 0, 3},
			{0, 0, 0, 0}
		};

		c = getopt_long(argc ,argv, "", long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
		case 1:
			test = atoi(optarg);
			break;
		case 2:
			time = atoi(optarg);
			break;
		case 3:
			usage(argc, argv);
			exit(0);
		default:
			usage(argc, argv);
			exit(-1);
		}
	}

	if (test == -1 || time == -1) {
		usage(argc, argv);
		exit(0);
	}

	fd = open(PAH8001_DEV, O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "couldn't open %s, %s\n", PAH8001_DEV, strerror(errno));
		return -1;
	}

	poweron(fd);
	switch (test) {
	case 1:
		err = pah8001_factory_testing_check_mechanical_assembly(fd, time);
		break;
	case 2:
		err = pah8001_factory_testing_check_frame_average(fd, time);
		break;
	default:
		usage(argc, argv);
		break;
	}
	poweroff(fd);

	close(fd);
	fd = -1;

	return err;
}
