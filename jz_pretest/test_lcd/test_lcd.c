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

#include <fcntl.h>
#include <stdio.h>

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <time.h>

#include <linux/fb.h>
#include <linux/kd.h>
//#include <linux/delay.h>

#include <pixelflinger/pixelflinger.h>

#include "minui.h"

typedef struct {
	GGLSurface texture;
	unsigned cwidth;
	unsigned cheight;
	unsigned ascent;
} GRFont;

static GGLContext *gr_context = 0;
static GGLSurface gr_framebuffer[2];
static unsigned gr_active_fb = 0;

static int gr_fb_fd = -1;
static int gr_vt_fd = -1;

static int buffercount = 0;

static struct fb_var_screeninfo vi;
struct fb_fix_screeninfo fi;
struct timespec tv, tv2;

static void dumpinfo(struct fb_fix_screeninfo *fi,
		     struct fb_var_screeninfo *vi);

static int get_framebuffer(GGLSurface * fb)
{
	int fd;
	void *bits;

	fd = open("/dev/graphics/fb0", O_RDWR);
	if (fd < 0) {
		printf
		    ("cannot open /dev/graphics/fb0, retrying with /dev/fb0\n");
		if ((fd = open("/dev/fb0", O_RDWR)) < 0) {
			perror("cannot open /dev/fb0");
			return -1;
		}
	}

	if (ioctl(fd, FBIOGET_FSCREENINFO, &fi) < 0) {
		perror("failed to get fb0 info");
		return -1;
	}

	if (ioctl(fd, FBIOGET_VSCREENINFO, &vi) < 0) {
		perror("failed to get fb0 info");
		return -1;
	}

	dumpinfo(&fi, &vi);

	bits = mmap(0, fi.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (bits == MAP_FAILED) {
		perror("failed to mmap framebuffer");
		return -1;
	}

	fb->version = sizeof(*fb);
	fb->width = vi.xres;
	fb->height = vi.yres;
	fb->stride = fi.line_length / (vi.bits_per_pixel >> 3);
	fb->data = bits;
	fb->format = GGL_PIXEL_FORMAT_RGB_565;

	fb++;

	fb->version = sizeof(*fb);
	fb->width = vi.xres;
	fb->height = vi.yres;
	fb->stride = fi.line_length / (vi.bits_per_pixel >> 3);
	//    fb->data = (void*) (((unsigned) bits) + vi.yres * vi.xres * 2);
	fb->data =
	    (void *)(((unsigned)bits) +
		     vi.yres * vi.xres * (vi.bits_per_pixel >> 3));
	fb->format = GGL_PIXEL_FORMAT_RGB_565;

	return fd;
}

static void set_active_framebuffer(unsigned n)
{
	if (n > 1)
		return;
	vi.yres_virtual = vi.yres * 2;
	vi.yoffset = n * vi.yres;

	if (ioctl(gr_fb_fd, FBIOPAN_DISPLAY, &vi) < 0) {
		fprintf(stderr, "active fb swap failed!\n");
	}
	// else
	//        printf("active buffer: %d\n", n);
}

static void dumpinfo(struct fb_fix_screeninfo *fi, struct fb_var_screeninfo *vi)
{
	printf("vi.xres = %d\n", vi->xres);
	printf("vi.yres = %d\n", vi->yres);
	printf("vi.xresv = %d\n", vi->xres_virtual);
	printf("vi.yresv = %d\n", vi->yres_virtual);
	printf("vi.xoff = %d\n", vi->xoffset);
	printf("vi.yoff = %d\n", vi->yoffset);
	printf("vi.bits_per_pixel = %d\n", vi->bits_per_pixel);

	printf("fi.line_length = %d\n", fi->line_length);
	printf("fi.smem_start = %d\n", (unsigned int)fi->smem_start);

}

int gr_init(void)
{
	int fd = -1;

	if (!access("/dev/tty0", F_OK)) {
		fd = open("/dev/tty0", O_RDWR | O_SYNC);
		if (fd < 0)
			return -1;

		if (ioctl(fd, KDSETMODE, (void *)KD_GRAPHICS)) {
			close(fd);
			return -1;
		}
	}

	gr_fb_fd = get_framebuffer(gr_framebuffer);

	if (gr_fb_fd < 0) {
		if (fd >= 0) {
			ioctl(fd, KDSETMODE, (void *)KD_TEXT);
			close(fd);
		}
		return -1;
	}

	gr_vt_fd = fd;

	/* start with 0 as front (displayed) and 1 as back (drawing) */
	gr_active_fb = 0;
	set_active_framebuffer(0);

	return 0;
}

void gr_exit(void)
{
	close(gr_fb_fd);
	gr_fb_fd = -1;

	if (gr_vt_fd >= 0) {
		ioctl(gr_vt_fd, KDSETMODE, (void *)KD_TEXT);
		close(gr_vt_fd);
		gr_vt_fd = -1;
	}
}

int gr_fb_width(void)
{
	return gr_framebuffer[0].width;
}

int gr_fb_height(void)
{
	return gr_framebuffer[0].height;
}

uint32_t red = 0xff0000;
uint32_t green = 0x00ff00;
uint32_t blue = 0x0000ff;
uint32_t black = 0x0;
uint32_t white = 0xffffff;

void clear_screen(int w, int h, uint32_t * loc)
{
	int i, j;
	int stride = fi.line_length / (vi.bits_per_pixel >> 3);
#if 0
	for (j = 0; j < h; j++)
		for (i = 0; i < w; i++)
			loc[i + j * (stride)] = 0x0;
#else
	for (j = 0; j < h; j++)
		for (i = 0; i < w; i++)
			loc[j * w + i] = 0x0;

#endif
}

unsigned char mem_flash[4090 * 4096];
void flash_dcache(void)
{
	memset(mem_flash, 0, sizeof(mem_flash));

}

void framebuffer_set_color(int w, int h, uint32_t * loc, unsigned int color)
{
	int i, j;
	int v;
	int stride = fi.line_length / (vi.bits_per_pixel >> 3);

	for (j = 0; j < h; j++) {
		for (i = 0; i < w; i++) {
			loc[i + j * (stride)] = color;
		}
	}
}

/*
 *红绿蓝黑白分别满屏显示
 */
void set_full_screen(void)
{
	int w;
	int h;
	int id = 0;
	int ret;
	int num;

	w = vi.xres;
	h = vi.yres;
	//  printf("w = %d, h = %d\n", w, h);
	//  fm_len = h*fi.line_length / (vi.bits_per_pixel >> 3);
	printf("vi.bits_per_pixel = %d\n", vi.bits_per_pixel);

	clear_screen(w, h, (uint32_t *) gr_framebuffer[1].data);

	int i;
	int j;
	int color[5] = { 0 };
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = black;
	color[4] = white;

	for (i = 0; i < 5; i++) {
		//draw_color(w, h, (uint32_t *)gr_framebuffer[id].data, color[i], &id);
		int id;
		buffercount++;
		id = buffercount%2;
		printf("i = %d, id = %d, buffercount =%d\n", i, id, buffercount);
		framebuffer_set_color(w, h, (uint32_t *) gr_framebuffer[id].data,
				      color[i]);
//		memcpy(gr_framebuffer[0].data, gr_framebuffer[1].data, fm_len);
		usleep(500000);
		set_active_framebuffer(id);
	}
}

/*
 *红绿蓝分屏
 */
void set_three_color(void)
{
	int w;
	int h;
	int id;
	w = vi.xres;
	h = vi.yres;
	buffercount++;
	id = buffercount%2;
	framebuffer_set_color(w, h/3, (uint32_t *) gr_framebuffer[id].data,
						  red);
	framebuffer_set_color(w, h/3, (uint32_t *) gr_framebuffer[id].data + h / 3  * w,
						  green);

	framebuffer_set_color(w, h/3, (uint32_t *) gr_framebuffer[id].data + 2 * h / 3 * w,
						  blue);
	usleep(500000);
//	memcpy(gr_framebuffer[0].data, gr_framebuffer[1].data, fm_len);
	set_active_framebuffer(id);
	usleep(50000);
}

/*在gr_framebuffer[bn]中存入小矩形 x，y是矩形坐标 w，h是矩形长宽，color为矩形颜色
 *
 */
void rectangle(int x, int y, int w, int h, unsigned int color,int bn)
{
	uint32_t * loc;
	loc = (uint32_t *)gr_framebuffer[bn].data + vi.xres * y + x;
	framebuffer_set_color(w, h, loc, color);
}

/*显示一个进度条,x,y为进度条左上第一个像素点坐标，w,h为进度条长高，frame_color为边框颜色,
 *fill_color为进度条颜色
 */
void progress_bar(int x, int y, int w, int h, unsigned int frame_color, unsigned int fill_color)
{
	int bar_x, bar_y, bar_w, bar_h;
	int fm_len;
	int frame_thickness = 1;
	int count = 0;
	int id;
	//framebuffer的大小:
//	fm_len = vi.xres * vi.yres * (vi.bits_per_pixel >> 3);
	clear_screen(w, h, (uint32_t *) gr_framebuffer[0].data);
	clear_screen(w, h, (uint32_t *) gr_framebuffer[1].data);

	//画一个矩形框
	for ( count = 0; count < 1; count++, buffercount++ )
	{
	bar_x = x;
	bar_y = y;
	bar_w = w;
	bar_h = h;
	id = buffercount%2;
	rectangle(bar_x, bar_y, bar_w, frame_thickness, frame_color, id);
	rectangle(bar_x, (bar_y + bar_h - 1), bar_w, frame_thickness, frame_color, id);
	rectangle(bar_x, bar_y, frame_thickness, bar_h, frame_color, id);
	rectangle((bar_x + bar_w -1), bar_y, frame_thickness, bar_h, frame_color, id);
	set_active_framebuffer(id);
	}
	//进度条
	bar_x = x + 1;
	bar_y = y + 1;
	bar_w = frame_thickness;
	bar_h = h - 2;
	for ( count = 0;count < ( w - 2 );count++, buffercount++ )
	{
		id = buffercount%2;
		rectangle(bar_x, bar_y, bar_w, bar_h, red, id);
		bar_w++;
		usleep(50000);
		set_active_framebuffer(id);
	}
	clear_screen(w, h, (uint32_t *) gr_framebuffer[0].data);
	clear_screen(w, h, (uint32_t *) gr_framebuffer[1].data);
}

/*
 *输出横向x个，纵向y个小方格
 *请使x，y能被屏幕分辨率整除
 */
void set_rectangle(int x, int y)
{
	int w;
	int h;
	int i, j, k, l, rec_w, rec_h;
	int id;
	k = x;
	l = y;
	w = vi.xres;
	h = vi.yres;
	rec_w = w / k;
	rec_h = h / l;

	clear_screen(w, h, (uint32_t *) gr_framebuffer[0].data);
	clear_screen(w, h, (uint32_t *) gr_framebuffer[1].data);
	buffercount++;
	id = buffercount%2;
	for (j = 0; j < y;) {
		for (i = 0; i < l;) {
			rectangle(i*rec_w, j*rec_h, rec_w, rec_h, white, id);
			i++;
			rectangle(i*rec_w, j*rec_h, rec_w, rec_h, black, id);
			i++;
		}
		j++;
		for (i = 0; i < k;) {
			rectangle(i*rec_w, j*rec_h, rec_w, rec_h, black, id);
			i++;
			rectangle(i*rec_w, j*rec_h, rec_w, rec_h, white, id);
			i++;
		}
		j++;
	}
	usleep(500*000);
	set_active_framebuffer(id);


}

int main(void)
{
	gr_init();
	set_active_framebuffer(0);
	usleep(50*1000);
	//红绿蓝黑白分别满屏显示
	set_full_screen();

	//红绿蓝分屏界面
	set_three_color();

	//20*20个黑白方格
	set_rectangle(20,20);

	//进度条
	//progress_bar((vi.xres/5), (vi.yres/4*3), (vi.xres/5*3), (vi.yres/8), white, red);

	return 0;
}
