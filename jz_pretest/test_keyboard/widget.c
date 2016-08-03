#include <stdio.h>
#include <linux/input.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include "widget.h"
#include "minui.h"
#include <sys/epoll.h>

#define DEV_PATH         "/dev/input/event0"
#define DEVICES_DRI_PATH "/proc/bus/input/devices"
#define MAXLINE          128
#define NAME             "gpio-keys"
#define HANDLERS         "Handlers"
#define MAXEPOLLSIZE     20
#define DEBUG
#ifdef DEBUG
#define DEBUG_LOG(...)
#else
#define DEBUG_LOG(...)
#endif
static pthread_mutex_t updateMutex;
static unsigned int widget_id;
static struct touch rec_touch;
static struct keyboard_touch kb_touch;
static FILE *err_fd;
static int width_limit;
static int height_limit;
static int swap = 0;

PNODE button_list = NULL;
PNODE keyboard_list = NULL;

int get_screen_width(){
	int a = gr_fb_width();
	return a;
}

int get_screen_height(){
	int a = gr_fb_height();
	return a;
}


int init_param(){
	gr_init();
	err_fd = fopen("/sbin/pretest/DEBUG_LOG.txt", "a+");
	pthread_mutex_init(&updateMutex, NULL);
	widget_id = 1;
	width_limit = get_screen_width();
	height_limit = get_screen_height();
	swap = 0;
	rec_touch.state = RESET_TOUCH;
	rec_touch.bt_id = 0;
	kb_touch.keyboard_id = 0;
	kb_touch.event_id = 0;
	kb_touch.state = RELEASE;
	return 0;
}

void set_swap(){
	if (swap == 1){
		swap = 0;
	}else{
		swap = 1;
	}
}

int init_backguound(char *name, int x, int y) {
	int err, iconWidth, iconHeight;
	gr_surface bg_surface;
	char resPath[256];
	snprintf(resPath, sizeof(resPath)-1, "../..%s", name);
	resPath[sizeof(resPath)-1] = '\0';
	err = res_create_display_surface(resPath, &bg_surface);
	iconWidth = gr_get_width(bg_surface);
	iconHeight = gr_get_height(bg_surface);
	gr_blit(bg_surface, 0, 0, iconWidth, iconHeight, x, y);
	gr_flip();
	return 0;
}

static int draw_default_button(struct button *button, int state) {
	struct button *mbutton = button;
	int x1, y1, x2, y2;
	unsigned char r, g, b, a;
	pthread_mutex_lock(&updateMutex);
	gr_color(0, 0, 0, 255);
	x1 = mbutton->default_button.x;
	y1 = mbutton->default_button.y;
	x2 = mbutton->default_button.x + mbutton->default_button.width;
	y2 = mbutton->default_button.y + mbutton->default_button.height;
	gr_fill(x1, y1, x2, y2);
	gr_color(76, 76, 76, 255);
	x1 = mbutton->default_button.x + 3;
	y1 = mbutton->default_button.y + 3;
	x2 = mbutton->default_button.x + mbutton->default_button.width - 3;
	y2 = mbutton->default_button.y + mbutton->default_button.height - 3;
	gr_fill(x1, y1, x2, y2);
	if (state == 0) {
		b = (unsigned char)((mbutton->default_button.rcolor >> 24) & (0x000000FF));
		g = (unsigned char)((mbutton->default_button.rcolor >> 16) & (0x000000FF));
		r = (unsigned char)((mbutton->default_button.rcolor >> 8) & (0x000000FF));
		a = (unsigned char)((mbutton->default_button.rcolor >> 0) & (0x000000FF));
		gr_color(b, g, r, a);
	} else if (state == 1) {
		b = (unsigned char) ((mbutton->default_button.pcolor >> 24) & (0x000000FF));
		g = (unsigned char) ((mbutton->default_button.pcolor >> 16) & (0x000000FF));
		r = (unsigned char) ((mbutton->default_button.pcolor >> 8) & (0x000000FF));
		a = (unsigned char) ((mbutton->default_button.pcolor >> 0) & (0x000000FF));
		gr_color(b, g, r, a);
	} else {
		DEBUG_LOG("draw_default_button fail");
		return -1;
	}
	x1 = mbutton->default_button.x + 5;
	y1 = mbutton->default_button.y + 5;
	x2 = mbutton->default_button.x + mbutton->default_button.width - 5;
	y2 = mbutton->default_button.y + mbutton->default_button.height - 5;
	gr_fill(x1, y1, x2, y2);
	gr_flip();
	pthread_mutex_unlock(&updateMutex);
	return 0;
}

static int draw_custom_button(struct button *button, int state) {
	struct button *mbutton = button;

	if (state == 0) {
		pthread_mutex_lock(&updateMutex);
		gr_blit(mbutton->release_surface, 0, 0, mbutton->rs.iconWidth,
				mbutton->rs.iconHeight, mbutton->rs.f_position_x,
				mbutton->rs.f_position_y);
		gr_flip();
		pthread_mutex_unlock(&updateMutex);
	} else if (state == 1) {
		pthread_mutex_lock(&updateMutex);
		gr_blit(mbutton->press_surface, 0, 0, mbutton->ps.iconWidth,
				mbutton->ps.iconHeight, mbutton->ps.f_position_x,
				mbutton->ps.f_position_y);
		gr_flip();
		pthread_mutex_unlock(&updateMutex);
	} else {
		DEBUG_LOG("draw_default_button fail");
		return -1;
	}
	return 0;
}

struct button *init_button(char *release_name, char *press_name, int f_position_x, int f_position_y){
	struct button *button1;
	int b_position[2];
	int err, iconWidth, iconHeight;
	char resPath[256];

	button1 = (struct button *)malloc(sizeof(struct button));
	button1->result=0;
	button1->style = CUSTOM;

	snprintf(resPath, sizeof(resPath)-1, "../..%s", release_name);
	resPath[sizeof(resPath)-1] = '\0';
	//res_create_surface打开图片并解析图片后，存入button中
	err = res_create_display_surface(resPath, &(button1->release_surface));
	snprintf(resPath, sizeof(resPath)-1, "../..%s", press_name);
	resPath[sizeof(resPath)-1] = '\0';
	//res_create_surface打开图片并解析图片后，存入button中
	err = res_create_display_surface(resPath, &(button1->press_surface));

	iconWidth = gr_get_width(button1->release_surface);
	iconHeight = gr_get_height(button1->release_surface);
	button1->release_name = release_name;
	button1->rs.f_position_x = f_position_x;
	button1->rs.f_position_y = f_position_y;
	button1->rs.iconWidth = iconWidth;
	button1->rs.iconHeight = iconHeight;

	iconWidth = gr_get_width(button1->press_surface);
	iconHeight = gr_get_height(button1->press_surface);
	button1->ps.f_position_x = f_position_x;
	button1->ps.f_position_y = f_position_y;
	button1->ps.iconWidth = iconWidth;
	button1->ps.iconHeight = iconHeight;
	button1->id = widget_id;
	widget_id++;
	button1->param = NULL;
	button1->func = NULL;
	button1->mode = NORMAL;
	button1->Draw_Button_In_End = ENABLE;
	button1->Fast_Touch = DISABLE;

	DEBUG_LOG( "start open %s\n, iconWidth=%d, iconHeight=%d", release_name,
			iconWidth, iconHeight);
	gr_blit(button1->release_surface, 0, 0, button1->rs.iconWidth,
			button1->rs.iconHeight, button1->rs.f_position_x,
			button1->rs.f_position_y);
	gr_flip();
	err = add_button(&button_list, button1);
	return button1;
}

struct button *init_default_button(int f_position_x, int f_position_y, int width, int height, unsigned int release_color, unsigned int press_color){
	struct button *button1;
	int b_position[2];
	int err;
	char resPath[256];

	button1 = (struct button *)malloc(sizeof(struct button));
	button1->result = 0;
	button1->style = DEFAULT;
	button1->default_button.x = f_position_x;
	button1->default_button.y = f_position_y;
	button1->default_button.width = width;
	button1->default_button.height = height;
	button1->default_button.rcolor = release_color;
	button1->default_button.pcolor = press_color;

	button1->rs.f_position_x = f_position_x;
	button1->rs.f_position_y = f_position_y;
	button1->rs.iconWidth = width;
	button1->rs.iconHeight = height;

	button1->ps.f_position_x = f_position_x;
	button1->ps.f_position_y = f_position_y;
	button1->ps.iconWidth = width;
	button1->ps.iconHeight = height;

	button1->id = widget_id;
	widget_id++;
	button1->param = NULL;
	button1->func = NULL;
	button1->mode = NORMAL;
	button1->Draw_Button_In_End = ENABLE;
	button1->Fast_Touch = DISABLE;

	DEBUG_LOG( "start open %s\n, iconWidth=%d, iconHeight=%d", release_name,
			iconWidth, iconHeight);
	draw_default_button(button1,0);
	err = add_button(&button_list, button1);
	return button1;
}

struct keyboard *init_keyboard(char *release_name, char *press_name, int f_position_x, int f_position_y){
	struct keyboard *keyboard1;
	int b_position[2];
	char resPath[256];
	int err, iconWidth, iconHeight;

	keyboard1 = (struct keyboard *)malloc(sizeof(struct keyboard));
	snprintf(resPath, sizeof(resPath)-1, "../..%s", release_name);
	resPath[sizeof(resPath)-1] = '\0';
	//res_create_surface打开图片并解析图片后，存入keyboard中
	err = res_create_display_surface(resPath, &(keyboard1->release_surface));
	snprintf(resPath, sizeof(resPath)-1, "../..%s", press_name);
	resPath[sizeof(resPath)-1] = '\0';
	//res_create_surface打开图片并解析图片后，存入keyboard中
	err = res_create_display_surface(resPath, &(keyboard1->press_surface));

	iconWidth = gr_get_width(keyboard1->release_surface);
	iconHeight = gr_get_height(keyboard1->release_surface);

	keyboard1->release_name = release_name;
	keyboard1->rs.f_position_x = f_position_x;
	keyboard1->rs.f_position_y = f_position_y;
	keyboard1->rs.iconWidth = iconWidth;
	keyboard1->rs.iconHeight = iconHeight;

	iconWidth = gr_get_width(keyboard1->press_surface);
	iconHeight = gr_get_height(keyboard1->press_surface);
	keyboard1->ps.f_position_x = f_position_x;
	keyboard1->ps.f_position_y = f_position_y;
	keyboard1->ps.iconWidth = iconWidth;
	keyboard1->ps.iconHeight = iconHeight;
	keyboard1->id = widget_id;
	widget_id++;
	DEBUG_LOG( "start open %s\n, iconWidth=%d, iconHeight=%d", release_name,
			iconWidth, iconHeight);
	gr_blit(keyboard1->release_surface, 0, 0, keyboard1->rs.iconWidth,
			keyboard1->rs.iconHeight, keyboard1->rs.f_position_x,
			keyboard1->rs.f_position_y);
	gr_flip();
	err = add_keyboard(&keyboard_list, keyboard1);
	return keyboard1;
}

//touch在button范围内返回1，不在返回2
int istouchingbutton(struct button button, struct touch touch){
	if ((touch.x > button.ps.f_position_x) && (touch.x < (button.ps.f_position_x+ button.ps.iconWidth)) &&
			(touch.y > button.ps.f_position_y) && (touch.y < (button.ps.f_position_y+ button.ps.iconHeight))) {
		return 1;
	}
		return 0;
}

//函数先从/proc/bus/input/devices中找到gpio-keys对应的event，打开key_event节点，返回fd
int open_keyboard_fd(){
	char event[8];
	char dev_path[25] = "/dev/input/";
	FILE *dir_fd;
	char line[MAXLINE];
	char *tmp;
	int keys_fd;
	dir_fd = fopen(DEVICES_DRI_PATH, "r");
	if (dir_fd == NULL) {
		DEBUG_LOG("open %s fail\n", DEVICES_DRI_PATH);
	}
	while (fgets(line, MAXLINE, dir_fd) != NULL) {
		if (strstr(line, NAME) != NULL) {
			while (fgets(line, MAXLINE, dir_fd) != NULL) {
				if (strstr(line, HANDLERS) != NULL) {
					tmp = strstr(line, "event");
					strlcpy(event, tmp, 8);
					strlcat(dev_path, event, 18);
					break;
				}
			}
			break;
		}
	}

	keys_fd = open(dev_path, O_RDONLY | O_NONBLOCK);
	if (keys_fd <= 0) {
		DEBUG_LOG("open %s device error!\n", dev_path);
		return -1;
	}
	return keys_fd;
}
//函数先从/proc/bus/input/devices中找到_ts(触摸屏都是以_ts结尾)对应的event，打开event节点，返回fd
int open_button_fd(){
	char event[8];
	char dev_path[25] = "/dev/input/";
	FILE *dir_fd;
	char line[MAXLINE];
	char *tmp;
	int keys_fd;
	dir_fd = fopen(DEVICES_DRI_PATH, "r");
	if (dir_fd == NULL) {
		DEBUG_LOG("open %s fail\n", DEVICES_DRI_PATH);
	}
	while (fgets(line, MAXLINE, dir_fd) != NULL) {
		if (strstr(line, "_ts") != NULL) {
			while (fgets(line, MAXLINE, dir_fd) != NULL) {
				if (strstr(line, HANDLERS) != NULL) {
					tmp = strstr(line, "event");
					strlcpy(event, tmp, 8);
					strlcat(dev_path, event, 18);
					break;
				}
			}
			break;
		}
	}

	keys_fd = open(dev_path, O_RDONLY | O_NONBLOCK);
	if (keys_fd <= 0) {
		DEBUG_LOG("open %s device error!\n", dev_path);
		return -1;
	}
	return keys_fd;
}


//根据当前button->state的值来画button的图片
static int draw_button(struct button *mbutton) {
	if (mbutton != NULL) {
		if (mbutton->state == KEEP_TOUCH) {
			if (mbutton->style == DEFAULT) {
				draw_default_button(mbutton, 1);
			} else if (mbutton->style == CUSTOM){
				draw_custom_button(mbutton, 1);
			}
		} else {
			if (mbutton->style == DEFAULT) {
				draw_default_button(mbutton, 0);
			} else if (mbutton->style == CUSTOM){
				draw_custom_button(mbutton, 0);
			}
		}
	} else {
		DEBUG_LOG("draw_button error");
	}
	return 0;
}

//信号为触摸信号返回EV_ABS（触摸事件），并将触摸事件的值填入给定的struct input_event *t中
//信号为同步信号返回EV_SYN，其他返回-1，
//swap == 1 时坐标旋转180
static int get_touch(struct input_event *t, struct touch *mtouch){


	if (t->type == EV_ABS) {
		mtouch->state = RESET_TOUCH;
		if (t->code == ABS_MT_POSITION_X) {
			mtouch->x = swap? (width_limit - t->value) : t->value;
		}
		if (t->code == ABS_MT_POSITION_Y) {
			mtouch->y = swap? (height_limit - t->value) : t->value;
		}

		return EV_ABS;
	}
	if (t->type == EV_SYN) {
		if(t->code == 0){
			if (mtouch->state == RESET_TOUCH)
				mtouch->state = KEEP_TOUCH;
			else if (mtouch->state == KEEP_TOUCH)
				mtouch->state = END_TOUCH;
			else
				mtouch->state = RESET_TOUCH;

			return EV_SYN;
		}
	}
	return -1;
}
//设置button的模式
int set_button_mode(struct button *button, enum button_mode mode){
	if (button != NULL){
	button->mode = mode;
	return 0;
	}else {
		DEBUG_LOG("set_button_mode fail");
	}
	return -1;
}
//通过当前触摸的位置，在button链表中寻找在此区域是否有button若有，将此button返回，若无返回NULL
struct button* find_button_by_position(struct touch mtouch, PNODE mlist) {
	PNODE mnode = mlist;
	struct button *mbutton;
	while (mnode != NULL) {
		mbutton = mnode->widget.button;
		if (istouchingbutton(*mbutton, mtouch) == 1){
			return mbutton;
		}
		mnode = mnode->pNext;
	}
	mbutton = NULL;
	return mbutton;
}
//通过button的id，在button链表中寻找是否有button，若有，将此button返回，若无返回NULL
struct button* find_button_by_id(int id, PNODE mlist){
	PNODE mnode = mlist;
	struct button *mbutton;
	while (mnode != NULL) {
		mbutton = mnode->widget.button;
		if (mbutton->id == id)
			return mbutton;
		mnode = mnode->pNext;
	}
	mbutton = NULL;
	return mbutton;
}
//捕捉触摸的轨迹，并确定是否有按下button，并根据触摸来画button图片
int catch_action_for_button(int fd, PNODE mlist){
	struct input_event t;
	struct button *mbutton, *last_button;
	int state;
	while (read(fd, &t, sizeof(t)) == sizeof(t)) {
		//驱动里一次触摸至少会上报8组数据，8组数据分两类，一类为EV_ABS是关于触摸位置，区域大小等信息
		//一类为EV_SYN。EV_SYN有两种含义，1：每一组触摸号结束都会发出，2：手抬起，也就是一连串触摸行为结束时会发出
		if (get_touch(&t,&rec_touch) == EV_SYN){
			switch (rec_touch.state) {
			case KEEP_TOUCH:  //当前触摸状态
				mbutton = find_button_by_position(rec_touch, mlist);
				if (mbutton == NULL) {
					last_button = find_button_by_id(rec_touch.bt_id, mlist);
					if (last_button == NULL) {
						rec_touch.bt_id = 0;
						break;
					}
					last_button->state = RESET_TOUCH;
					if (last_button->Draw_Button_In_End == 1) {
						draw_button(last_button);
						gr_flip();
					} // 决定是否画上次最后触摸的button

					rec_touch.bt_id = 0;
					break;
				}
				if (mbutton != NULL) {
					last_button = find_button_by_id(rec_touch.bt_id, mlist);
					if (last_button == NULL)
					{
						mbutton->state = KEEP_TOUCH;
						draw_button(mbutton);
						gr_flip();
						rec_touch.bt_id = mbutton->id;
						if (mbutton->Fast_Touch == 1) {
							return rec_touch.bt_id;
						}
						break;
					} else if (last_button != NULL) {
						if (last_button->id != mbutton->id) {
							last_button->state = RESET_TOUCH;
							if (last_button->Draw_Button_In_End == 1) {
								draw_button(last_button);
								gr_flip();
							} // 决定是否画上次最后触摸的button

							mbutton->state = KEEP_TOUCH;
							draw_button(mbutton);
							gr_flip();
						}
					}
					rec_touch.bt_id = mbutton->id;
				}
				if (mbutton->Fast_Touch == 1) {
					return rec_touch.bt_id;
				}
				break;
			case END_TOUCH:  //当前触摸状态
				mbutton = find_button_by_id(rec_touch.bt_id, mlist);
				if (mbutton == NULL) {
					return rec_touch.bt_id;
				} else {
					mbutton->state = RESET_TOUCH;
					if (mbutton->Draw_Button_In_End != 0) {
					draw_button(mbutton);
					gr_flip();
					}  //决定结束时是否画release状态的button

					return rec_touch.bt_id;
				}
				break;
			default:
				DEBUG_LOG("catch_action_for_button error\n");
			}
		}
	}
	return 0;
}
//获得按键状态
static int get_keyboard_touch(struct input_event *t, struct keyboard_touch *mkb_touch){
	if (t->type == EV_KEY){
		mkb_touch->event_id = t->code;
		mkb_touch->state = t->value;
		return EV_KEY;
	}
	if (t->type == EV_SYN) {
		return EV_SYN;
	}
	return -1;
}
//捕捉按键行为，并确定是否有按下按键，并根据次来画按键图片
int catch_action_for_keyboard(int fd, PNODE mlist){
	struct input_event t;
	struct keyboard *mkeyboard;
	PNODE mnode;
	while (read(fd, &t, sizeof(t)) == sizeof(t)) {
		mnode = mlist;
		while (mnode != NULL) {
			mkeyboard = mnode->widget.keyboard;
			if (t.type == EV_KEY) {
				if (t.value == 0) {
					pthread_mutex_lock(&updateMutex);
					gr_blit(mkeyboard->release_surface, 0, 0,
							mkeyboard->rs.iconWidth,
							mkeyboard->rs.iconHeight,
							mkeyboard->rs.f_position_x,
							mkeyboard->rs.f_position_y);
					gr_flip();
					pthread_mutex_unlock(&updateMutex);
					gr_flip();

				}
				if (t.value == 1) {
					pthread_mutex_lock(&updateMutex);
					gr_blit(mkeyboard->press_surface, 0, 0,
							mkeyboard->ps.iconWidth,
							mkeyboard->rs.iconHeight,
							mkeyboard->ps.f_position_x,
							mkeyboard->ps.f_position_y);
					gr_flip();
					pthread_mutex_unlock(&updateMutex);
					gr_flip();
				}
			}
			mnode = mnode->pNext;
		}
	}
	return 0;
}
//监听按键和触摸行为，这个函数会阻塞，所以若有需要可以在新的线程执行
//函数通过epoll同时监控多个input上报事件是否有相应，并根据响应来执行对应的回调函数
int start_listen(){
	struct keyboard *keyboard_tmp;
	struct button *bt_tmp;
	int bt_fd, epfd, key_fd;
	int nfd;
	int i, err, id;
	struct epoll_event ev, events[20];

	key_fd = open_keyboard_fd();
	bt_fd = open_button_fd();
	epfd = epoll_create(MAXEPOLLSIZE);
	ev.events = EPOLLIN;
	ev.data.fd = bt_fd;
	err = epoll_ctl(epfd, EPOLL_CTL_ADD, ev.data.fd, &ev);
	if (err < 0) {
		fprintf(stderr, "epoll set insertion error: fd=%d\n", ev.data.fd);
		return -1;
	}
	ev.events = EPOLLIN;
	ev.data.fd = key_fd;
	err = epoll_ctl(epfd, EPOLL_CTL_ADD, ev.data.fd, &ev);
	if (err < 0) {
		fprintf(stderr, "epoll set insertion error: fd=%d\n", ev.data.fd);
		return -1;
	}
	while (1) {
		nfd = epoll_wait(epfd, events, 20, -1);
		if (nfd == -1) {
			perror("epoll_wait");
			continue;
		}
		for (i = 0; i < nfd; ++i) {
			if (events[i].data.fd == bt_fd) {
				id = catch_action_for_button(bt_fd, button_list);
				bt_tmp = find_button_by_id(id, button_list);
				if (bt_tmp != NULL) {
					bt_tmp->result = 1;
					if (bt_tmp->func != NULL) {
						if (bt_tmp->param != NULL) {
							bt_tmp->func(bt_tmp->param);
						} else {
							bt_tmp->func(0);
						}
					}
					if (bt_tmp->mode == ECS_LISTEN) {
						return 0;
					}
				}
			} else if (events[i].data.fd == key_fd) {
				catch_action_for_keyboard(key_fd, keyboard_list);
			}
		}
	}
	return 0;
}


