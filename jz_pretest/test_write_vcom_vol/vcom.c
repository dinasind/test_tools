/*
 * Copyright 2013, Ingenic Semiconductor
 *
 *     Author: Aaron Wang <hfwang@ingenic.cn>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <pthread.h>
#include <errno.h>
#include <cutils/log.h>

struct partition {
    char name[32];
    unsigned int offset;
    unsigned int size;
    int type;
};

#define INAND_PINFO_FILENAME    "/sys/class/mmc_host/mmc0/device/partitions"
#define BLOCK_DEV_NAME          "/dev/block/mmcblk0"
#define RECOVERY_CMD_FILE       "/cache/recovery/command"
#define LARGE_BUF_LEN           512

#define PROP_PARTS_ALLOC        "ro.partition.num"
#define PROP_DEVID_BYTES        "ro.devid.bytes"
#define PROP_WIFIMAC_OFFSET     "ro.wifimac.offset"
#define PROP_WIFIMAC_BYTES      "ro.wifimac.bytes"
#define PROP_BTMAC_OFFSET       "ro.btmac.offset"
#define PROP_BTMAC_BYTES        "ro.btmac.bytes"

#define PROP_BT_MAC             "ro.bt.mac"
#define PROP_WIFI_MAC           "ro.wifi.mac"
#define PROP_SERIAL_NO          "ro.serial.no"

static int parts_alloc = 0, devid_bytes = 0;
static int wifimac_offset = 0, wifimac_bytes = 0;
static int btmac_offset = 0, btmac_bytes = 0;

static struct partition *pinfo = NULL;

static int read_data_emmc(struct partition *pinfo, char *buf, int size)
{

	int ret = 0, fd = 0;
	unsigned int pos = 0;

	if (!pinfo) {
		ALOGE("Argument is NULL!\n");
		return -EINVAL;
	}

	fd = open(BLOCK_DEV_NAME, O_RDONLY);
	if (fd < 0) {
		ALOGE("Open %s error, errno =  %d\n",BLOCK_DEV_NAME, errno);
		goto err_open;
	}

	pos = pinfo->offset;

	lseek(fd, pos, SEEK_SET);

	ret = read(fd, buf, size);
	buf[ret] = '\0';

	if (ret < 0) {
		ALOGE("Read error, errno = %d\n", errno);
		goto err_read;
	}

err_read:
	close(fd);
err_open:
	return ret;
}

static int write_data_emmc(struct partition *pinfo, char *buf, int size)
{

	int ret = 0, fd = 0;
	unsigned int pos = 0;

	if (!pinfo) {
		ALOGE("Argument is NULL!\n");
		return -EINVAL;
	}

	fd = open(BLOCK_DEV_NAME, O_WRONLY);
	if (fd < 0) {
		ALOGE("Open %s error, errno =  %d\n",BLOCK_DEV_NAME, errno);
		goto err_open;
	}

	pos = pinfo->offset;

	lseek(fd, pos, SEEK_SET);

	ret = write(fd, buf, size);
	printf("finished the second step,Kid\n");

	if (ret < 0) {
		ALOGE("Write error, errno = %d\n", errno);
		goto err_write;
	}

err_write:
	close(fd);
err_open:
	return ret;
}

static int scan_partition_info_emmc(void)
{
	char *buf = NULL, *bufp = NULL, part_name[32];
	int ret = 0, nbytes = 0, i = 0, fd = 0;
	int matches,part_saddr,part_size,part_type;

	bufp = buf = malloc(LARGE_BUF_LEN);
	if (NULL == bufp) {
		ALOGE("No memory for malloc!\n");
		return -ENOMEM;
	}

	memset(buf, 0, LARGE_BUF_LEN);

	fd = open(INAND_PINFO_FILENAME, O_RDONLY);
	if (fd < 0) {
		ALOGE(" Open %s errno =  %d!\n",INAND_PINFO_FILENAME, errno);
		goto err_open;
	}

	lseek(fd, 0, SEEK_SET);

	nbytes = read(fd, buf, LARGE_BUF_LEN);
	if (nbytes < 0) {
		ALOGE("Read error, errno = %d\n",errno);
		ret = errno;
		goto err_read;
	}

	while (nbytes > 0) {
		part_name[0] = '\0';

		matches = sscanf(buf, "%s %x %x %d\n",
				part_name, &part_saddr, &part_size, &part_type);

		if (matches == 4) {
			if (!strncmp(part_name,"device_id",strlen("device_id"))) {
				strncpy(pinfo[0].name,part_name,sizeof("device_id"));
				pinfo[0].offset = part_saddr;
				pinfo[0].type = part_type;

				strncpy(pinfo[1].name,"wifi_mac",sizeof("wifi_mac"));
				pinfo[1].offset = part_saddr + wifimac_offset,
				pinfo[1].type = part_type;

				strncpy(pinfo[2].name,"bt_mac",sizeof("bt_mac"));
				pinfo[2].offset = part_saddr + btmac_offset,
				pinfo[2].type = part_type;
			} else if (!strncmp(part_name,"battery",strlen("battery"))) {
				strncpy(pinfo[parts_alloc - 1].name,part_name,sizeof("battery"));
				printf("Finished the first step,Kid\n");
				pinfo[parts_alloc - 1].offset = part_saddr;
				pinfo[parts_alloc - 1].type = part_type;
			}
		}

		/* eat the line */
		while (nbytes > 0 && *buf != '\n') {
			buf++;
			nbytes--;
		}

		if (nbytes > 0) {
			buf++;
			nbytes--;
		}
	}

	for (i = 0; i < parts_alloc; i ++) {
		ALOGD("%s  \t0x%08x",pinfo[i].name, pinfo[i].offset);
	}

err_read:
	close(fd);
err_open:
	free(bufp);
	return ret;
}

static int set_devid_prop(void)
{
	int i = 0, ret = 0;
	char dev_prop[64] = { 0 };

	for (i = 0; i < parts_alloc; i ++) {
		if (!strncmp(pinfo[i].name, "device_id" , strlen("device_id"))) {
			ret = read_data_emmc(&pinfo[i], dev_prop, devid_bytes);
			if (ret < 0) {
				ALOGE("Read device id error, errno = %d\n",ret);
				property_set(PROP_SERIAL_NO, "UNKNOWN");
				goto err_read_data;
			} else {
				property_set(PROP_SERIAL_NO, dev_prop);
			}

			break;
		}
	}
	return 0;

err_read_data:
	return ret;
}

static int set_wifi_mac_prop(void)
{
	int i = 0, ret = 0;
	char dev_prop[64] = { 0 };

	for (i = 0; i < parts_alloc; i ++) {
		if (!strncmp(pinfo[i].name, "wifi_mac" , strlen("wifi_mac"))) {
			ret = read_data_emmc(&pinfo[i], dev_prop, wifimac_bytes);
			if (ret < 0) {
				ALOGE("Read wifi mac address error, errno = %d\n",ret);
				property_set(PROP_WIFI_MAC, "UNKNOWN");
				goto err_read_data;
			} else {
				property_set(PROP_WIFI_MAC, dev_prop);
			}
			break;
		}
	}
	return 0;

err_read_data:
	return ret;
}

static int set_bt_mac_prop(void)
{
	int i = 0, ret = 0;
	char dev_prop[64] = { 0 };

	for (i = 0; i < parts_alloc; i ++) {
		if (!strncmp(pinfo[i].name, "bt_mac" , strlen("bt_mac"))) {
			ret = read_data_emmc(&pinfo[i], dev_prop, btmac_bytes);
			if (ret < 0) {
				ALOGE("Read bt mac address error, errno = %d\n",ret);
				property_set(PROP_BT_MAC, "UNKNOWN");
				goto err_read_data;
			} else {
				property_set(PROP_BT_MAC, dev_prop);
			}
		}
	}
	return 0;

err_read_data:
	return ret;
}

static int write_vcom_voltage(char *buf,int len)
{
	int i = 0, ret = 0;
	//char buf[64];

	//memset(buf, 0 , len);

	for (i = 0; i < 5/*parts_alloc*/; i ++) {
		if (!strncmp(pinfo[i].name, "battery" , strlen("battery"))) {
			ret = write_data_emmc(&pinfo[i], buf, sizeof(buf));
			if (ret < 0) {
				ALOGE("Write error, errno = %d\n",ret);
				goto err_write_data;
			}
		}
	}
	return 0;

err_write_data:
	return ret;
}

static int set_flag_recovery(void)
{
	int i = 0, ret = 0, fd = 0, size = 0;
	char buf[64];
	fd = open(RECOVERY_CMD_FILE, O_RDWR | O_TRUNC | O_CREAT, 0777);
	if (fd < 0) {
		ALOGE("Open %s error, errno = %d\n",RECOVERY_CMD_FILE,errno);
		return errno;
	}

	size = sizeof("--wipe_data\n");
	memset(buf, 0, sizeof(buf));
	strncpy(buf,"--wipe_data\n",size);
	ret = write(fd, buf, size);
	if (ret != size) {
		ALOGE("Write error, errno = %d\n", errno);
		goto err_write;
	}

err_write:
	close(fd);

	return ret;
}

int main(int argc, char *argv[])
{
	int ret = 0, size = 0, erase_flag = 1;
	char prop[16] = { 0 };

	char buf[LARGE_BUF_LEN];
	int i;

	memset(buf,0,LARGE_BUF_LEN);
	for(i = 0; i < 4; ++i) {
		buf[i] = argv[1][i];
	}

#if 0
	if (argc > 1) {
		if (!strcmp(argv[1],"erase_flag_ftest")) {
			/* for erase factory test flag */
			erase_flag = 1;
		} else if (!strcmp(argv[1],"set_flag_recovery")) {
			/* for set recovery flag
			 * when power off
			 * enter recovery mode
			 * wipe data and cache
			 */
			set_flag_recovery();
			return 0;
		}
	}
#endif
#if 0
	property_get(PROP_PARTS_ALLOC, prop, NULL);
	parts_alloc = strtoul(prop, 0, 10);

	property_get(PROP_DEVID_BYTES, prop, NULL);
	devid_bytes = strtoul(prop, 0, 10);

	property_get(PROP_WIFIMAC_OFFSET, prop, NULL);
	wifimac_offset = strtoul(prop, 0, 10);

	property_get(PROP_WIFIMAC_BYTES, prop, NULL);
	wifimac_bytes = strtoul(prop, 0, 10);

	property_get(PROP_BTMAC_OFFSET, prop, NULL);
	btmac_offset = strtoul(prop, 0, 10);

	property_get(PROP_BTMAC_BYTES, prop, NULL);
	btmac_bytes = strtoul(prop, 0, 10);

	ALOGD("parts_alloc    = %d\n",parts_alloc);
	ALOGD("devid_bytes    = %d\n",devid_bytes);
	ALOGD("wifimac_offset = %d\n",wifimac_offset);
	ALOGD("wifimac_bytes  = %d\n",wifimac_bytes);
	ALOGD("btmac_offset   = %d\n",btmac_offset);
	ALOGD("btmac_bytes    = %d\n",btmac_bytes);
#endif
//	if (parts_alloc > 0) {

	printf("parts_alloc = %d,Kid\n",parts_alloc);

	size = parts_alloc * sizeof(struct partition);
	pinfo = malloc(size);
	memset(pinfo, 0, size);

	ret = scan_partition_info_emmc();
	if (ret < 0) {
		ALOGE("Scan emmc partition error, errno = %d\n",ret);
		goto err_scan;
	}
//	} else {
//		goto err_scan;
//	}
/*
	if (devid_bytes > 0) {
		ret = set_devid_prop();
		if (ret < 0) {
			ALOGE("Set device id prop error, errno = %d\n",ret);
		}
	}

	if (wifimac_bytes > 0) {
		ret = set_wifi_mac_prop();
		if (ret < 0) {
			ALOGE("Set wifi mac prop error, errno = %d\n",ret);
		}
	}

	if (btmac_bytes > 0) {
		ret = set_bt_mac_prop();
		if (ret < 0) {
			ALOGE("Set bt mac prop error, errno = %d\n",ret);
		}
	}

*/
	if (erase_flag) {
	ret = write_vcom_voltage(buf,LARGE_BUF_LEN);
		if (ret < 0) {
			ALOGE("Erase flag error, errno = %d\n",ret);
		}
	}

err_scan:
	if (pinfo)
		free(pinfo);
	return ret;
}
