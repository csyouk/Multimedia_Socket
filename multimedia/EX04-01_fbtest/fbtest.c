#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#define FB_NUM0	0
#define FB_NUM1	1

#define APP_NAME "fbtest"
#define app_info(...) {printf(APP_NAME" info: "); printf(__VA_ARGS__);}
#define app_err(...) {printf(APP_NAME" error: "); printf(__VA_ARGS__);}

#define S3CFB_WIN_SET_PLANE_ALPHA	_IOW ('F', 204, struct s3cfb_user_plane_alpha)

#define COLOR_RED ((0xff<<16)|(0x0<<8)|(0x0<<0))
#define COLOR_GREEN ((0x0<<16)|(0xff<<8)|(0x0<<0))
#define COLOR_BLUE ((0x0<<16)|(0x0<<8)|(0xff<<0))
#define COLOR_BLACK ((0x0<<16)|(0x0<<8)|(0x0<<0))
#define COLOR_WHITE ((0xff<<16)|(0xff<<8)|(0xff<<0))

struct s3cfb_user_plane_alpha {
	int channel;
	unsigned char red;
	unsigned char green;
	unsigned char blue;
};

int draw_rect(int x, int y, int w, int h, unsigned long color, struct fb_var_screeninfo *vip, struct fb_fix_screeninfo *fip, char *map)
{
	int xx, yy;
	long location = 0;

	for(yy = y; yy < (y+h); yy++) {
		for(xx = x; xx < (x+w); xx++) {
			location = (xx+vip->xoffset) * (vip->bits_per_pixel/8) +
				(yy+vip->yoffset) * fip->line_length;
			if (vip->bits_per_pixel == 32) {
				*(unsigned long *)(map + location) = color;
			} else  { /* 16bpp */
				*(unsigned short *)(map + location) = (unsigned short)color;
			}
		}
	}

	return 0;
}

int main(void)
{
	int fd_fb0;
	int fd_fb1;
	int fd;

	long size_fb0;
	long size_fb1;

	char *map_fb0;
	char *map_fb1;

	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
	int x = 0, y = 0;
	long location = 0;
	char temp_buf[64];
	struct s3cfb_user_plane_alpha user_alpha;

	/* FB_NUM0: open */
	sprintf(temp_buf, "/dev/fb%d", FB_NUM0);
	fd_fb0 = open(temp_buf, O_RDWR);
	if(fd_fb0 == -1) {
		app_err("cannot open %s\n", temp_buf);
		exit(-1);
	}
	/* FB_NUM0: get fb_var_screeninfo */
	if (ioctl(fd_fb0, FBIOGET_VSCREENINFO, &vinfo) == -1) {
		app_err("cannot read FBIOGET_VSCREENINFO\n");
		exit(-1);
	}
	/* FB_NUM0: get fb_fix_screeninfo */
	if (ioctl(fd_fb0, FBIOGET_FSCREENINFO, &finfo) == -1) {
		app_err("cannot read FBIOGET_FSCREENINFO\n");
		exit(-1);
	}
	app_info("%s was opened successfully\n", temp_buf);
	app_info("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);
	/* FB_NUM0: mmap */
	size_fb0 = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
	map_fb0 = (char *)mmap(0, size_fb0, PROT_READ | PROT_WRITE, MAP_SHARED, fd_fb0, 0);
	if ((int)map_fb0 == -1) {
		app_err("cannot map %s to memory\n", temp_buf);
		exit(-1);
	}
	app_info("%s was mapped to %p\n", temp_buf, map_fb0);
	/* FB_NUM0: draw rectangle */
	draw_rect(vinfo.xoffset, vinfo.yoffset, vinfo.xres, vinfo.yres, COLOR_BLACK, &vinfo, &finfo, map_fb0);
	draw_rect(300, 160, 200, 160, COLOR_RED, &vinfo, &finfo, map_fb0);

	/* FB_NUM1: open */
	sprintf(temp_buf, "/dev/fb%d", FB_NUM1);
	fd_fb1 = open(temp_buf, O_RDWR);
	if(fd_fb1 == -1) {
		app_err("cannot open %s\n", temp_buf);
		exit(-1);
	}
	/* FB_NUM1: get fb_var_screeninfo */
	if (ioctl(fd_fb1, FBIOGET_VSCREENINFO, &vinfo) == -1) {
		app_err("cannot read FBIOGET_VSCREENINFO\n");
		exit(-1);
	}

	/* FB_NUM1: set fb_var_screeninfo */
	vinfo.activate |= FB_ACTIVATE_FORCE;
	vinfo.xres = 400;
	vinfo.yres = 240;
	vinfo.xres_virtual = 400;
	vinfo.yres_virtual = 240;
	vinfo.xoffset = 0;
	vinfo.yoffset = 0;
	if (ioctl(fd_fb1, FBIOPUT_VSCREENINFO, &vinfo) == -1) {
		app_err("cannot write FBIOPUT_VSCREENINFO\n");
		exit(-1);
	}
	/* FB_NUM1: get fb_fix_screeninfo */
	if (ioctl(fd_fb1, FBIOGET_FSCREENINFO, &finfo) == -1) {
		app_err("cannot read FBIOGET_FSCREENINFO\n");
		exit(-1);
	}

	app_info("%s was opened successfully\n", temp_buf);
	app_info("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);
	/* FB_NUM1: mmap */
	size_fb1 = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
	map_fb1 = (char *)mmap(0, size_fb1, PROT_READ | PROT_WRITE, MAP_SHARED, fd_fb1, 0);
	if ((int)map_fb1 == -1) {
		app_err("cannot map %s to memory\n", temp_buf);
		exit(-1);
	}
	app_info("%s was mapped to %p\n", temp_buf, map_fb1);

	/* FB_NUM1: draw rectangle */
	draw_rect(vinfo.xoffset, vinfo.yoffset, vinfo.xres, vinfo.yres, COLOR_BLACK, &vinfo, &finfo, map_fb1);
	draw_rect(250, 90, 150, 150, COLOR_BLUE|0x80000000, &vinfo, &finfo, map_fb1);

	/* wait */
	printf("Sleeping 5 seconds\n");
	sleep(5);
	printf("Finished.\n");

	/* close */
	munmap(map_fb1, size_fb1);
	munmap(map_fb0, size_fb0);
	close(fd_fb1);
	close(fd_fb0);

	return 0;
}
