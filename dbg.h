/* 2021 RedHate (Ultros) */
/* framebuffer graphics library */

#define RED 					0xFFFF0000
#define GREEN 					0xFF00FF00
#define BLUE					0xFF0000FF
#define YELLOW 					0xFFFFFF00
#define PURPLE 					0xFFFF00FF
#define CYAN 					0xFF00FFFF
#define WHITE 					0xFFFFFFFF
#define GRAY 					0xFFCCCCCC
#define DARKGRAY 				0xFF888888
#define DARKERGRAY 				0xFF444444
#define BLACK 					0xFF000000
#define NOCOLOR 				0

#define ALPHA_RED 				0xCCFF0000
#define ALPHA_GREEN 			0xCC00FF00
#define ALPHA_BLUE				0xCC0000FF
#define ALPHA_BLACK 			0xCC000000
#define ALPHA_WHITE 			0xCCFFFFFF
#define ALPHA_GRAY 				0xCCCCCCCC
#define ALPHA_DARKGRAY 			0xCC888888
#define ALPHA_DARKERGRAY 		0xCC444444
#define ALPHA_BLACK 			0xCC000000
#define ALPHA_BLACK_MEDIUM 		0x88000000
#define ALPHA_BLACK_LIGHT		0x44000000

/**
 * Debug fonts
 */
#define DEBUG_FONT_DEBUG		0
#define DEBUG_FONT_ACORN		1
#define DEBUG_FONT_PERL			2
#define DEBUG_FONT_SPARTA		3
#define DEBUG_FONT_LINUX		4
#define DEBUG_FONT_LUCIDIA		5

#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <malloc.h>
#include "stb_image.h"
#include "dbgfnt.h"

typedef struct dbg_ctx{
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
	unsigned int *fbp;
	long int screensize;
	int fbfd;
}dbg_ctx;

typedef struct img{
	int w,h,c;
	unsigned char *data;
}img;

class dbg{

	private:
	
		//one only
		dbg_ctx context;

	public:
	
		/**
		 * Open and map the framebuffer pointer and retreive screen info
		 */
		int  init(){

			//Open the framebuffer for reading and writing
			context.fbfd = open("/dev/fb0", O_RDWR);
			if (context.fbfd == -1){
				printf("Error: cannot open framebuffer device\r\n");
				return 0;
			}
			printf("The framebuffer device was opened successfully.\r\n");

			//Get fixed screen information
			if (ioctl(context.fbfd, FBIOGET_FSCREENINFO, &context.finfo) == -1){
				printf("Error reading fixed information\r\n");
				return 0;
			}

			//Get variable screen information
			if (ioctl(context.fbfd, FBIOGET_VSCREENINFO, &context.vinfo) == -1){
				printf("Error reading variable information\r\n");
				return 0;
			}

			printf("%dx%d, %dbpp\r\n", context.vinfo.xres, context.vinfo.yres, context.vinfo.bits_per_pixel);
			context.vinfo.xres=context.vinfo.xres+10; //chromebook fixup, the above functions do NOT return the proper framebuffer dimensions.

			//Figure out the size of the screen in bytes
			context.screensize = (context.vinfo.xres * (context.vinfo.bits_per_pixel / 8)) * context.vinfo.yres;

			//Map the device to memory
			context.fbp = (unsigned int*)mmap(0, context.screensize, PROT_READ | PROT_WRITE, MAP_SHARED, context.fbfd, 0);
			if (context.fbp == NULL){
				printf("Error: failed to map framebuffer device to memory\r\n");
				return 0;
			}
			printf("The framebuffer device was mapped to memory successfully.\r\n running...\r\n");
			
			return 1;
		}
		/**
		 * Unmap and close the framebuffer pointer 
		 */
		void shutdown(){
			munmap(context.fbp, context.screensize);
			close(context.fbfd);
		}
		/**
		 * Clear screen with color
		 * #define RED 						0xFFFF0000
		 * #define GREEN 					0xFF00FF00
		 * #define BLUE						0xFF0000FF
		 * #define YELLOW 					0xFFFFFF00
		 * #define PURPLE 					0xFFFF00FF
		 * #define CYAN 					0xFF00FFFF
		 * #define WHITE 					0xFFFFFFFF
		 * #define GRAY 					0xFFCCCCCC
		 * #define DARKGRAY 				0xFF888888
		 * #define DARKERGRAY 				0xFF444444
		 * #define BLACK 					0xFF000000
		 * #define NOCOLOR 					0
		 * #define ALPHA_RED 				0xCCFF0000
		 * #define ALPHA_GREEN 				0xCC00FF00
		 * #define ALPHA_BLUE				0xCC0000FF
		 * #define ALPHA_BLACK 				0xCC000000
		 * #define ALPHA_WHITE 				0xCCFFFFFF
		 * #define ALPHA_GRAY 				0xCCCCCCCC
		 * #define ALPHA_DARKGRAY 			0xCC888888
		 * #define ALPHA_DARKERGRAY 		0xCC444444
		 * #define ALPHA_BLACK 				0xCC000000
		 * #define ALPHA_BLACK_MEDIUM 		0x88000000
		 * #define ALPHA_BLACK_LIGHT		0x44000000
		 */
		void clear(unsigned int color){
			unsigned int i; for(i=0; i<context.vinfo.xres * context.vinfo.yres; i++) context.fbp[i] = color;
		}
		/**
		 * Print (blit) a message to vram using debug fonts
		 * msg - const u8 string
		 * x - x offset
		 * y - y offset
		 * fg_col - foreground color
		 * bg_col - background_color
		 * selected_font - self explainitory
		 * ... - vsnprintf tail's 
		 * 
		 * #define DEBUG_FONT_DEBUG			0
		 * #define DEBUG_FONT_ACORN			1
		 * #define DEBUG_FONT_PERL			2
		 * #define DEBUG_FONT_SPARTA		3
		 * #define DEBUG_FONT_LINUX			4
		 * #define DEBUG_FONT_LUCIDIA		5
		 * 
		 */
		void print(const char *msg, int x, int y, int fg_col, int bg_col, int selected_font, ...){

			char buf[256];
			va_list ap;
			va_start(ap, msg);
			vsnprintf(buf, 256, msg, ap);
			va_end(ap);
			buf[255] = 0;

			unsigned int a,b,p;
			int offset;
			char code;

			y = y * 7; //if set to 8 it creates a 1px space between rows, i assume due to 0 counted as interger in the loops thus making it 8.

			unsigned char font;
			unsigned int pwidth 		= (context.vinfo.xres*(context.vinfo.bits_per_pixel / 8));
			unsigned int bufferwidth 	= context.vinfo.xres;

			for(a=0;buf[a] && a<pwidth;a++){
				code = buf[a] & 0x7f;
				for(b=0;b<7;b++){
					offset = ((y+b)*bufferwidth)+((x+a)*8);
					font = msx[selected_font][ code*8 + b ];
					for(p=0;p<8;p++){
						context.fbp[offset] = (font & 0x80) ? fg_col : bg_col;
						font <<= 1;
						offset++;
					}
				}
			}

		}
		/**
		 * file - "/path/to/file"
		 */
		img load_img(const char *file){

			img a_img; //= (img*) malloc(sizeof(img));
			
			int w,h,c,i;
			unsigned char *data = stbi_load(file, &w, &h, &c, 0);
			
			switch(c){
				case 3:
					for(i=0;i<w*3*h;i+=3){
						unsigned char tmp = data[i+0];
						data[i+0] = data[i+2];
						data[i+2] = tmp;
					}
				break;
				case 4:
					for(i=0;i<w*4*h;i+=4){
						unsigned char tmp = data[i+0];
						data[i+0] = data[i+2];
						data[i+2] = tmp;
					}
				break;
			}
			
			a_img.data = data;
			a_img.w = w;
			a_img.h = h;
			a_img.c = c;

			printf("stbimg %d %d %d\r\n", w,h,c);
				
			return a_img;

		}
		/**
		 * i - image
		 * x - x offset
		 * y - y offset
		 */
		void draw_img(img i, int x, int y){
			
			unsigned int pwidth	= (context.vinfo.xres*(context.vinfo.bits_per_pixel / 8));
			unsigned int imgpwidth = (i.w*i.c);
			unsigned char *scr = (unsigned char*) context.fbp;
			unsigned int px_fmt = (context.vinfo.bits_per_pixel / 8);
			
			int sy,sx;
			for(sy=0; sy<i.h; sy++){ //for height
				for(sx=0; sx<i.w; sx++){ //for width
					//screen is 4 bytes, the image is "i.c" bytes
					memcpy(&scr[(y*px_fmt)+(x*pwidth)+(sx*px_fmt)+(pwidth*sy)], (void*)&i.data[(sx*i.c)+(imgpwidth*sy)], i.c);
				}
			}
			
		}
		/**
		 * i - image
		 */
		void free_img(img i){
			free(i.data);
		}

};

