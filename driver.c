//
//  driver.c
//  Project 1: Graphics Library
//
//  Created by Christian Jarani on 9/13/17.
//  Copyright © 2017 majula. All rights reserved.
//
//  keep pressing e to draw more lines

#include "graphics.h"

int main(int argc, char **argv)
{
	init_graphics();
	void *buf = new_offscreen_buffer();

	char key; 
	int width, rHeight = 10, gHeight = 40, bHeight = 70;		// line width, red/blue/green line height

	draw_line(buf,0,rHeight,200,rHeight,RGB(31,0,0));
	draw_line(buf,0,gHeight,200,gHeight,RGB(0,63,0));
	draw_line(buf,0,bHeight,200,bHeight,RGB(0,0,31));

	do {
		key = getkey();
		if (key == 'q')
			break;
		else if(key == 'e'){
			draw_line(buf,0,++rHeight,200,rHeight,RGB(31,0,0));
			draw_line(buf,0,++gHeight,200,gHeight,RGB(0,63,0));
			draw_line(buf,0,++bHeight,200,bHeight,RGB(0,0,31));
			blit(buf);
		}
		sleep_ms(200);
	}
	while (1);

	exit_graphics();
	return 0;
}