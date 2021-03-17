#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include "dbg.h"

int main(){

	dbg screen;

	//init
	if(!screen.init()) return 0;
	
	//debug clear
	screen.clear(NOCOLOR);
	
	//load the image
	img bg_img = screen.load_img("test.png");
	
	//draw the image
	screen.draw_img(bg_img,  ((768/2)-(bg_img.h/2)), ((1376/2)-(bg_img.w/2)));
	
        //debug print stuff
	screen.print("Hello From Debug!", 1, 1, RED, NOCOLOR, DEBUG_FONT_DEBUG);
	screen.print("Hello From Debug!", 1, 2, RED, NOCOLOR, DEBUG_FONT_ACORN);
	screen.print("Hello From Debug!", 1, 3, RED, NOCOLOR, DEBUG_FONT_PERL);
	screen.print("Hello From Debug!", 1, 4, RED, NOCOLOR, DEBUG_FONT_SPARTA);
	screen.print("Hello From Debug!", 1, 5, RED, NOCOLOR, DEBUG_FONT_LINUX);
	screen.print("Hello From Debug!", 1, 6, RED, NOCOLOR, DEBUG_FONT_LUCIDIA);

	//cleanup
	screen.free_img(bg_img);

	//shut down
	screen.shutdown();
	
	return 0;
	
}

