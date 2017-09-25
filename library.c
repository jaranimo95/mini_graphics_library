//
//  main.c
//  gLib
//
//  Created by Christian Jarani on 9/13/17.
//  Copyright © 2017 majula. All rights reserved.
//

#include "graphics.h"
#include <stdio.h>

int file_descriptor;
void* file_address;

int    vres;		// virtual resolution
int	   depth;		// bit depth
size_t length;		// length of memory allocation in mmap

void init_graphics()
{
	struct fb_fix_screeninfo fix_info;							// Lets us get info about our video card's memory
	struct fb_var_screeninfo var_info;							// Lets us get info about our video card's display properties
	struct termios t;

	file_descriptor = open("/dev/fb0", O_RDWR);					// Open framebuffer
	if(file_descriptor == -1) {									// Check if frambuffer was opened properly
		perror("/dev/fb0 not opening properly.");					// If not, say so
	}
	else perror("shit worked yo");									// Else we GOOD BOIIIIIII

	ioctl(file_descriptor, FBIOGET_FSCREENINFO, &fix_info);
	ioctl(file_descriptor, FBIOGET_VSCREENINFO, &var_info);	
	 depth = fix_info.line_length;
	  vres = var_info.yres_virtual; 
 	length = depth * vres;

 	file_address = mmap(0,length,PROT_WRITE,MAP_SHARED,file_descriptor,0); // Map file to memory and get that #hot #wet #address #space #babYYYYYYYY
 	//exit_graphics() if memory isn't allocated properly i.e. file_address == (void*) -1

 	ioctl(0,TCGETS,&termios);


}

void exit_graphics() {}
char getkey() {}
void sleep_ms(long ms) {}
void* new_offscreen_buffer() {}

void blit(void *src) {}
void clear_screen(void *img) {}
void draw_pixel(void *img, int x, int y, color_t color) {}
void draw_line(void *img, int x1, int y1, int width, int height, color_t c) {}

int main() {
	init_graphics();
	return 0;
}
