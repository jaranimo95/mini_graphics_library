//
//  driver.c
//  gLib
//
//  Created by Christian Jarani on 9/15/17.
//  Copyright © 2017 majula. All rights reserved.
//

#ifndef HEADER_FILE
#define HEADER_FILE

#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/select.h>

typedef unsigned short color_t;
#define RGB(r,g,b) (r << 11 | g << 5 | b) // RGB color macro, with red being RGB(31,0,0)

void  init_graphics();
void  exit_graphics();
char  getkey();
void  sleep_ms(long);
void* new_offscreen_buffer();
void  blit(void *);
void  clear_screen(void*);
void  draw_pixel(void*, int, int, color_t);
void  draw_line(void*, int, int, int, int, color_t);

#endif
