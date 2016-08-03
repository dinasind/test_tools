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

#include "../test_keyboard/widget.h"
#include "minui.h"
#include "../test_keyboard/list.h"

#define BUTTON_WIDTH_NUMBER       5                  //横向button个数
#define BUTTON_HEIGHT_NUMBER      5                  //竖向button个数


struct para {
	struct button *this_bt;
	struct button **bt_list;
	int len;
};

//检查button是否都被触摸过，若是，则将当前的button设置为退出状态
void check_bt(void *para) {
	struct para *mpara = (struct para *) para;
	struct button *mthis_bt = mpara->this_bt;
	struct button **mbt_list = mpara->bt_list;
	int i = 0;
	for (i = 0; i < mpara->len; i++) {
		if (mbt_list[i]->result == 0) {
			return;
		}
	}

	set_button_mode(mthis_bt, ECS_LISTEN);
	return;
}

int main() {
	int F1;
	char *p;
	int width;
	int height;
	int bt_num = BUTTON_WIDTH_NUMBER * BUTTON_HEIGHT_NUMBER;
	struct button *bt_pass[bt_num];
	int i = 0, j = 0;
	struct para mpara[bt_num];

	init_param();
	p = getenv("F1");
	F1 = atoi(p);
	if (F1 == 1)
		set_swap();
	width = get_screen_width()/5;
	height = get_screen_height()/5;
	for (j = 0; j < BUTTON_HEIGHT_NUMBER; j++) {
		for (i = 0; i < BUTTON_WIDTH_NUMBER; i++) {
			bt_pass[i+j*5] = init_default_button(i*width, j*height, width, height, GREEN, RED);
			bt_pass[i+j*5]->Draw_Button_In_End = DISABLE;
			bt_pass[i+j*5]->Fast_Touch = ENABLE;
			bt_pass[i+j*5]->func = check_bt;
			mpara[i+j*5].len = bt_num;
			mpara[i+j*5].bt_list = bt_pass;
			mpara[i+j*5].this_bt = bt_pass[i+j*5];
			bt_pass[i+j*5]->param = &mpara[i+j*5];
		}
	}
	//set_button_mode(bt_pass[],ECS_LISTEN);
	start_listen();
	return 0;
}


