//
//  library.c
//  Project 1: Graphics Library
//
//  Created by Christian Jarani on 9/13/17.
//  Copyright © 2017 majula. All rights reserved.
//

#include "graphics.h"

int file_descriptor;	// /dev/fb0 descriptor
void* fb_address;		// /dev/fb0 address
struct termios t;		// Grants access to key press info

int    vres;			// virtual resolution
int	   depth;			// bit depth
size_t length;			// length of memory allocation in mmap

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

 	fb_address = mmap(0,length,PROT_WRITE,MAP_SHARED,file_descriptor,0);  	// Map file to memory and get that #hot #wet #address #space #babYYYYYYYY
 	if(fb_address == (void*) -1) {											// If memory isn't allocated properly
 		perror("! ERROR ! - framebuffer memory map failed");					// Print error message
 		exit_graphics();													// Exit graphics program
 	}

 	ioctl(0,TCGETS,&t);				// Get information about keypress
 	t.c_lflag &= ~(ECHO | ICANON);	// Disable keypress
 	ioctl(0,TCSETS,&t);				// Set keypress to disabled

 	write(1, "\033[2J", 4);			// Clear terminal
}

void exit_graphics() 
{
	t.c_lflag |= (ECHO | ICANON);	// Re-enable keypress
	ioctl(0,TCSETS,&t);				// Set keypress to enabled
	munmap(fb_address, length);		// Unmap memory allocated to framebuffer
	close(file_descriptor);			// Close framebuffer
	write(1, "\033[2J", 4);			// Clear terminal
}

char getkey() 
{
	struct timeval clock;	// We need a clock to check our elapsed time for select() syscall
	fd_set key_status;		// Gives us status of key press
	char input = 0;			// Set to "no key pressed" status
	clock.tv_sec = 0;		// Check immediately for key press status (~0 seconds elapse before checking)

	FD_ZERO(&key_status);										// Initialize file descriptor
  	FD_SET(0, &key_status);										// Check stdin for input
  	if(select(FD_SETSIZE, &key_status, NULL, NULL, &clock))		// If key pressed
	  	read(0, &input, 1);											// Put "key pressed" status in input
  	return input;												// Return input
}

void sleep_ms(long ms)
{
	struct timespec alarm;					// Make an alarm so we can wake ourselves up
	alarm.tv_sec = ms / 1000;				// Set for however many whole seconds (milliseconds / 1000)
	alarm.tv_nsec = (ms % 1000) * 1000000;	// Then convert the remaining time to nanoseconds
  	nanosleep(&alarm, NULL);				// Sleep for whole seconds (tv_sec) + leftover time in nanoseconds (tv_nsec)
}

void* new_offscreen_buffer()
{
	void* ob_address = mmap(0,length,PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);  	// Map file to memory and get that #hot #wet #address #space #babYYYYYYYY
	if(ob_address == (void*) -1) {													// If offscreen buffer memory isn't allocated properly
 		perror("! ERROR ! - offscreen buffer memory map failed");						// Print error message
 		exit_graphics();																// Exit graphics program
 	}
 	else return ob_address;
}

void blit(void* src)
{
	int i;
	for(i = 0; i < length; i++){					// Copy each byte from src image (offscreen buffer) to the framebuffer
		((char*)fb_address)[i] = ((char*)src)[i];
	}
}

void draw_pixel(void *img, int x, int y, color_t c) 
{
	color_t* pixels;
  	if(x < 0 || y < 0 || x >= 640 || y >= 480) {	// Screen boundary check (found with fbset -s command)
    	perror("Position is out of bounds");			// If they are, say so
    	exit_graphics();								// And cleanup all necessary stuff to exit
  	}

  	pixels = img;									// Masks file address as color_t*
  	int position = x + (y * (depth/2));				// Calculates the correct position in the array from x and y
  	pixels[position] = c;							// Sets corresponding pixel to chosen color
}

// from: https://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm#C
void draw_line(void *img, int x0, int y0, int x1, int y1, color_t c)
{
	int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
  	int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1;
  	int err = (dx>dy ? dx : -dy)/2, e2;
 
  	for(;;){
   		draw_pixel(img,x0,y0,c);
    	if (x0==x1 && y0==y1) break;
    	e2 = err;
    	if (e2 >-dx) { err -= dy; x0 += sx; }
    	if (e2 < dy) { err += dx; y0 += sy; }
  	}
}

void clear_screen(void* img)
{
	int i;
	for(i = 0; i < length; i++){	// Set each byte of the buffer to 0
		((char*)img)[i] = 0;
	}
}
