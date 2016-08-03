/*
 * test_touchbuttom.c
 *
 *  Created on: 2016-3-31
 *      Author: haikangzhou
 */
#include <stdio.h>
#include <linux/input.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "../test_keyboard/widget.h"
#include "minui.h"
#include "../test_keyboard/list.h"

#define CURRENT_PATH        "/sys/class/power_supply/battery/current_now"    //电流节点
#define STANDARD_CURRENT    100                                              //标准电流大小

int main() {

	struct button *bt_next;
	int x;
	int fd = -1;
	int charge_flag = 0;
	int flag = -1;
	int ret = -1;
	int F1;
	char *p=NULL;
	char buff[20];

	init_param();
	p = getenv("F1");
	F1 = atoi(p);
	if (F1 == 1)
		set_swap();

	x = get_screen_width();                                            //获得屏幕大小
	init_backguound("/system/pretest/currenttest", (x - 242)/2, 30);   //“电流测试”图片置中
	bt_next = init_button("/system/pretest/rnextitem", "/system/pretest/pnextitem", (x - 142)/2, 240);
	set_button_mode(bt_next, ECS_LISTEN);
	fd = open(CURRENT_PATH, O_RDONLY);

	if (fd <= 0) {
		printf("open %s device error!\n", CURRENT_PATH);
		flag = -1;
	}
	while (1) {                                        //获得电流大小
		read(fd, buff, 5);
		lseek(fd,0,SEEK_SET);
		ret = atoi(buff);
		printf("current = %d,buff=%s",ret,buff);
		if ((ret <= 0) && (charge_flag == 0)) {
			charge_flag = -1;
			init_backguound("/system/pretest/stopcharge", (x - 242)/2, 130);   //-1时为充电状态 提示拔下电源

		}
		if ((ret > 0) && (ret < STANDARD_CURRENT)){                             //电流小于标准，通过
			flag = 0;
			break;
		}
		if (ret > STANDARD_CURRENT) {                                           //失败
			flag = 1;
			break;
		}
	}

	if (flag == 0){
		init_backguound("/system/pretest/currentsuccess", (x - 242)/2, 130);
	} else {
		init_backguound("/system/pretest/currentfail", (x - 242)/2, 130);
	}

	start_listen();
	close(fd);
	return flag;
}


