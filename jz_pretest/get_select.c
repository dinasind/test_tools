#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <time.h>

#include <linux/fb.h>
#include <linux/kd.h>

#include <pixelflinger/pixelflinger.h>

static struct fb_var_screeninfo vi;

/*
 *获得屏幕像素大小信息
 */
static int get_screeninfo()
{
	int fd;
	fd = open("/dev/graphics/fb0", O_RDWR);
	if (ioctl(fd, FBIOGET_VSCREENINFO, &vi) < 0) {
		perror("failed to get fb0 info");
		return -1;
	}
	return 0;
}

/*
 *解析触摸的坐标，判断触摸的按键,屏幕如下
 ****************
 *              *
 *              *
 *              *
 *              *
 *              *
 ****************
 * 成功   * 失败  *
 ****************
 *当点击成功区域坐标时返回1，失败返回2，其他坐标返回3
 *f1版本会旋转屏幕，所以需要坐标全部反转180
 */
int button_select(int x, int y)
{
	int xres = vi.xres;
	int yres = vi.yres;
	char *p=NULL;
	int F1;
	p = getenv("F1");
	F1 = atoi(p);
	if(F1 == 1)
	{
		if((y < (yres*1/4)) && (x > xres/2))
		{
			return 1;
		}else if((y < (yres*1/4)) && (x < xres/2))
		{
			return 2;
		}
		return 3;
	}else{
		if((y > (yres*3/4)) && (x < (xres/2)))
		{
			return 1;
		}else if((y > (yres*3/4)) && (x > xres/2))
		{
			return 2;
		}
		return 3;
	}
	return 3;
}

int main(int argc, char *argv[])
{
	FILE *fp;
	char buf[50];
	int row;
	int x = 0, y = 0;
	char *co_str;
	char x_co_str[50];
	char y_co_str[50];
	char x_co[]="0003 0035";/*在getevent中，触摸的x坐标信息的前八位为0003 0035*/
	char y_co[]="0003 0036";/*在getevent中，触摸的y坐标信息的前八位为0003 0036*/

	get_screeninfo();
	if (argc != 2) {
		printf("args error, use: get_coordinate [log_file] ");
		return -1;
	}
	/*从[log_file]获取getevent的信息，解析触摸的x,y坐标信息并打印出来，成功返回0，
	 *若不成功或没有解析到触摸信号返回1
	 */
	memset(buf, 0, sizeof(buf));
	fp = fopen(argv[1], "r");
	if (fp == NULL) {
		perror("argv[1]\n");
		printf("open file error\n");
		return -1;
	}
	for (row = 0; row < 5; ) {
		if (fgets(buf, sizeof(buf), fp) == NULL) {
			fclose(fp);
			return 4;
		}

		row ++;
		if (strstr(buf, x_co) != NULL) {
			co_str = strstr(buf, x_co);
			if(co_str == NULL) {
				return -1;
			}
			strcpy(x_co_str, (co_str + 10));
			x = (int)strtol(x_co_str,NULL,16);
		}
		if (strstr(buf, y_co) != NULL) {
			co_str = strstr(buf, y_co);
			if(co_str == NULL) {
				return -1;
			}
			strcpy(y_co_str, (co_str + 10));
			y = (int)strtol(y_co_str,NULL,16);
		}
		if (feof(fp))
			break;

		memset(buf, 0, sizeof(buf));
	}
	fclose(fp);
	return button_select(x,y);
}
