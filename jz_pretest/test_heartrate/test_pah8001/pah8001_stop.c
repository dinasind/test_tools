#include <stdio.h>
#include <linux/input.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/epoll.h>

#include "../../test_keyboard/widget.h"
#include "minui.h"
#include "../../test_keyboard/list.h"



int main() {
	struct keyboard *keyboard1;
	struct button *bt_pass;




	init_param();
	init_backguound("/system/pretest/heartratefail", 0, 0);
	bt_pass = init_button("/system/pretest/rNextItem", "/system/pretest/pNextItem", 89, 255);

	set_button_mode(bt_pass,ECS_LISTEN);
	start_listen();

	return 1;
}
