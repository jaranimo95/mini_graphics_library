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

struct termios t;

void init_graphics()
{
	struct fb_fix_screeninfo fix_info;							// Lets us get info about our video card's memory
	struct fb_var_screeninfo var_info;							// Lets us get info about our video card's display properties

	file_descriptor = open("/dev/fb0", O_RDWR);					// Open framebuffer
	if(file_descriptor == -1)									// Check if frambuffer was opened properly
		perror("!ERROR ! - /dev/fb0 not opened properly");			// If not, say so

	ioctl(file_descriptor, FBIOGET_FSCREENINFO, &fix_info);		// Allows us to access bit depth of the native video card
	ioctl(file_descriptor, FBIOGET_VSCREENINFO, &var_info);		// Allows us to access virtual resolution of our video card
	depth = fix_info.line_length;
	vres = var_info.yres_virtual; 
 	length = depth * vres;										

 	file_address = mmap(0,length,PROT_WRITE,MAP_SHARED,file_descriptor,0);  // Map file to memory and get that #hot #wet #address #space #babYYYYYYYY
 	if(file_address == (void*) -1) {										// If memory isn't allocated properly
 		perror("! ERROR ! - Memory map failed");							// Print error message
 		exit_graphics();													// Exit graphics program
 	}

 	ioctl(0,TCGETS,&t);					// Get information about keypress
 	t.c_lflag &= ~(ECHO | ICANON);		// Disable keypress
 	ioctl(0,TCSETS,&t);					// Set keypress to disabled

 	//clear screen here
}

void exit_graphics() 
{
	t.c_lflag |= (ECHO | ICANON);	// Re-enable keypress
	ioctl(0,TCSETS,&t);				// Set keypress to enabled
	munmap(file_address, length);
	close(file_descriptor);			// Close framebuffer
}

char getkey() 
{
	struct timeval clock;	// We need a clock to check the time
	fd_set key_status;		// Gives us status of key press
	char input = 0;			// Set to "no key pressed" status


}

void clear_screen(void *img) 
{
	print(1,"\033[2J",4);		// Clears terminal, thus, clearing the screen
}

void sleep_ms(long ms) {}
void* new_offscreen_buffer() {}

void blit(void *src) {}
void draw_pixel(void *img, int x, int y, color_t color) {}
void draw_line(void *img, int x1, int y1, int width, int height, color_t c) {}

int main() {
	init_graphics();
	return 0;
}
