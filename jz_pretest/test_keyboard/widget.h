#ifndef _PRETEST_WIDGET_H_
#define _PRETEST_WIDGET_H_
#include <stdio.h>
#include <linux/input.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdint.h>
#include "minui.h"
#include "list.h"

#define RED             0x0000ffff
#define GREEN           0x00ff00ff
#define BLUE            0xff0000ff
#define BLACK           0x000000ff
#define WHITE           0xffffffff

PNODE button_list;
PNODE keyboard_list;

enum touch_state {
	END_TOUCH = 2,
	KEEP_TOUCH = 1,
	RESET_TOUCH = 0
};
enum keyboard_state {
	RELEASE = 0,
	PRESS = 1
};

struct keyboard_touch {
	int keyboard_id;
	int event_id;
	enum keyboard_state state;
};
struct touch {
	unsigned int x;
	unsigned int y;
	enum touch_state state;
	int bt_id;
};

struct position {
	unsigned int f_position_x;
	unsigned int f_position_y;
	int iconWidth;
	int iconHeight;
};

struct keyboard {
	int id;
	int event_id;
	char *release_name;
	char *press_name;
	enum touch_state state;
	int result;
	gr_surface release_surface;
	gr_surface press_surface;
	struct position rs;
	struct position ps;
};

enum button_mode {
ECS_LISTEN,
NORMAL

};

enum button_style {
	DEFAULT,
	CUSTOM
};

enum enable_or_disable {
	DISABLE = 0,
	ENABLE = 1
};

struct default_button_style {
	int x;
	int y;
	int width;
	int height;
	int rcolor;
	int pcolor;
};
//param，func是回调函数的参数和回调函数，若需要button执行某些函数，请先初始化button再填充func函数指针，
//若func有参数，可以将参数放入结构体中，并将结构体地址指针赋值给param。
struct button {
	int id;
	enum button_mode mode;
	enum button_style style;
	enum enable_or_disable Draw_Button_In_End;//设为ENABLE时：按完按钮后按钮的是release状态，DISABLE：按完按钮后按钮是press状态
	enum enable_or_disable Fast_Touch; //设为ENABLE时：只要触摸到马上执行触摸后的行为，DIABLE：触摸结束和触摸开始时为同一按钮才认定为触摸了按钮
	struct default_button_style default_button;
	char *release_name;
	char *press_name;
	enum touch_state state;
	int result;
	gr_surface release_surface; //未按下按键时的图片
	gr_surface press_surface;   //按下按键时的图片
	struct position rs;
	struct position ps;
	void *param;
	void (*func)(void *);
};

//init_param();函数用来初始化一些参数，使用button和key之前先调用这个函数
int init_param();

//init_backguound用来初始化背景图片，name是图片的绝对目录地址，x，y为图片的位置（也就是图片左上角第一个像素的位置）
//如图片的目录为/sbin/pretest/backguound.png,图片第一个像素在（20，30）的位置，
//则：init_backguound（"/sbin/pretest/backguound",20,30）;
int init_backguound(char *name, int x, int y);

//按键的初始化与init_backguound类似，将按键按下时，和未按下时的照片地址，坐标填充，函数会返回一个
//被初始化好的keyboard
struct keyboard *init_keyboard(char *release_name, char *press_name, int f_position_x, int f_position_y);

//按键的初始化与init_backguound类似，将按键按下时，和未按下时的图片地址，坐标填充，函数会返回一个
//被初始化好的button
struct button *init_button(char *release_name, char *press_name, int f_position_x, int f_position_y);

//初始化一个默认样式的button，
struct button *init_default_button(int f_position_x, int f_position_y, int width, int height, unsigned int release_color, unsigned int press_color);

//可以设置button的模式，模式有两种，使用方法将button，和模式分别填入：
//1：ECS_LISTEN：button在被点击后会结束监听，进而结束程序
//2：NORMAL：button被点击后不会结束监听
int set_button_mode(struct button *button, enum button_mode mode);

//获得屏幕宽
int get_screen_width();
//获得屏幕高
int get_screen_height();
//初始化好各个button后，开始监听，

//触摸的效果将旋转180
void set_swap();
int start_listen();
#endif
