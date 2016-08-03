#include <stdio.h>
#include <linux/input.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/epoll.h>

#include "widget.h"
#include "minui.h"
#include "list.h"



int check_button_result(struct button *button) {
	return button->result;
}

int main() {
	struct keyboard *keyboard1;
	struct button *bt_pass, *bt_fail;




	init_param();
	init_backguound("/system/pretest/testkey", 40, 40);
	keyboard1 = init_keyboard("/system/pretest/releasekey","/system/pretest/presskey", 130, 168);
	bt_pass = init_button("/system/pretest/rpass", "/system/pretest/ppass", 55, 300);
	bt_fail = init_button("/system/pretest/rfail", "/system/pretest/pfail", 205, 300);

	set_button_mode(bt_pass,ECS_LISTEN);
	set_button_mode(bt_fail,ECS_LISTEN);
	start_listen();

	return check_button_result(bt_fail);
}
