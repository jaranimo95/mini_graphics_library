//CS 1550 – Project 1: Double-Buffered Graphics Library
//Due: Sunday, October 1, 2017, by 11:59pm

//Project Description
    Your instructor was learning ARM assembly language over a prior summer break and was trying to make some projects for CS/COE 0447 to do. While text-based programs are okay, it’s really the graphical programs that make it a bit more interesting. So your instructor started to write a graphics library so the students could make a simple game.

    In doing so, he noticed that a lot of the basic functionality the library provided was accessing the low-level operating system features that we talk about in 1550. The light bulb went off over his head, and project 1 was born

    In this project, you’ll be writing a small graphics library that can set a pixel to a particular color, draw some basic shapes, and read keypresses.

//How It Will Work
    You will provide the following library functions:

        - void init_graphics()        -- Syscalls Used: open, ioctl, mmap
        - void exit_graphics()        -- Syscalls Used: ioctl
        - char getkey()               -- Syscalls Used: select, read
        - void sleep_ms(long ms)      -- Syscalls Used: nanosleep
        - void new_offscreen_buffer() -- Syscalls Used: mmap

        - void blit()
        - void clear_screen(void *img)
        - void draw_pixel(void *img, int x, int y, color_t color)
        - void draw_line(void *img, int x1, int y1, int width, int height, color_t c)

//Function Descriptions

    //init_graphics()
        1. You will need to open the graphics device. The memory that backs a video display is often called a framebuffer. In the version of Linux we are using, the kernel has been built to allow you direct access to the framebuffer. As we learned in 449, hardware devices in Unix/Linux are usually exposed to user programs via the /dev/ filesystem. In our case, we will find a file, /dev/fb0, that represents the first (zero-th) framebuffer attached to the computer.

           Since it appears to be a file, it can be opened using the open() syscall. To set a pixel, we only need to do basic file operations. You could seek to a location and then write some data. However, we will have a better way.

        2. We can do something special to make writing to the screen easier than using a whole bunch of seeks and writes. Since each of those is a system call on their own, using them to do a lot of graphics operations would be slow. Instead, we can ask the OS to map a file into our address space so that we can treat it as an array and use loads and stores to manipulate the file contents. We will be covering this more later when we discuss memory management, but for now, we want to use this idea of memory mapping for our library. The mmap() system call takes a file descriptor from open (and a bunch of other parameters) and returns a void *, an address in our address space that now represents the contents of the file. That means that we can use pointer arithmetic or array subscripting to set each individual pixel, provided we know how many pixels there are and how many bytes of data is associated with each pixel.

            One note is that we must use the MAP_SHARED parameter to mmap() because other parts of the OS want to use the framebuffer too.

        3. In order to use the memory mapping we have just established correctly, we need some information about the screen. We need to know the resolution (number of pixels) in the x and y directions as well as the bit-depth: the amount of colors we can use. The machine that QEMU is configured to emulate is in a basic 640x480 mode with 16-bit color. This means that there are 640 pixels in the x direction (0 is leftmost and 639 is rightmost) and 480 pixels in the y direction (0 is topmost, 479 is bottommost). In 16-bit color, we store the intensity of the three primary colors of additive light, Red, Green, and Blue, all packed together into a single 16-bit number.  Since 16 isn’t evenly divisible by three (RGB), we devote the upper 5 bits to storing red intensity (0-31), the middle 6 bits to store green intensity (0-63), and the low order 5 bits to store blue intensity (0-31).

            You can then use typedef to make a color type, color_t, that is an unsigned 16-bit value. You must also make a macro named RGB to encode a color_t from three RGB values using bit shifting and masking to make a single 16-bit number.

            To get the screen size and bits per pixels, we can use a special system call: ioctl. This system call is used to query and set parameters for almost any device connected to the system. You pass it a file descriptor and a particular number that represents the request you’re making of that device. We will use two requests: FBIOGET_VSCREENINFO and FBIOGET_FSCREENINFO. The first will give back a struct fb_var_screeninfo that will give the virtual resolution. The second will give back a struct fb_fix_screeninfo from which we can determine the bit depth. The total size of the mmap()’ed file would be the yres_virtual field of the first struct multiplied by the line_length field of the second.

        4. We will use an ANSI escape code to clear the screen rather than blanking it by drawing a giant rectangle of black pixels. ANSI escape codes are a sequence of characters that are not meant to be displayed as text but rather interpreted as commands to the terminal. We can print the string “\033[2J” to tell the terminal to clear itself. Note that this uses an octal escape sequence, where \033 is only one character.
                                                                                                                                                                                                                                                                                                                 
        5. Our final step is to use the ioctl system call to disable keypress echo (displaying the keys as you’re typing on the screen automatically) and buffering the keypresses. The commands we will need are TCGETS and TCSETS. These will yield or take a struct termios that describes the current terminal settings. You will want to disable canonical mode by unsetting the ICANON bit and disabling ECHO by forcing that bit to zero as well.
                                                                                                                                                                                                                                                                                                        
        6. There is one problem with this. When the terminal settings are changed, the changes last even after the program terminates. That’s why we have the exit_graphics() call. It can clean up the terminal settings by restoring them to what they were prior to us changing them.
                                                                                                                                                                                                                                                                                                                 
           That handles the normal termination, but it’s always possible that our program abnormally terminates, i.e., it crashes. In that case when you try to type something at the commandline, it will not show up on the screen. What you’re typing is still working, but you cannot see it. A reboot will fix this, but I found it useful to make a little helper program I called “fix” to turn echo back on in this event.

                                                                                                                                                                                                                                                                                                                 
    //exit_graphics()
        1.  This is your function to undo whatever it is that needs to be cleaned up before the program exits. Many things will be cleaned up automatically if we forget, for instance files will be closed and memory can be unmapped. It’s always a good idea to do it yourself with close() and munmap() though. What you’ll definitely need to do is to make an ioctl() to reenable key press echoing and buffering as described above.

                                                                                                                                                                                                                                                                                                                 
    //getkey()
        1. To make games, we probably want some sort of user input. We will use key press input and we can read a single character using the read() syscall. However, read() is blocking and will cause our program to not draw unless the user has typed something. Instead, we want to know if there is a keypress at all, and if so, read it.
                                                                                                                                                                                                                                                                                                                
           This can be done using the Linux non-blocking syscall select().
                                                                                                                                                                                                                                                                                                                
    
    //sleep_ms()
        1. We will be using the syscall nanosleep() to make our program sleep between frames of graphics being drawn. From its name, you can guess that it has nanosceond precision, but we dont need that level of granularity. We will instead sleep for a specified number of milliseconds and just multiply that by 1000000.

           We do not need to worry about the call being interrupted, and, so, the second parameter to nanosleep() can be NULL.

                                                                                                                                                                                                                                                                                                                 
    //new_offscreen_buffer()
        1. If we draw directly to the framebuffer, a complex scene may render slow enough to see each part appear on screen. To get the illusion of more fluid graphics, a technique known as "double buffering" is often used. We make a duplicate array of pixels in RAM rather than representing any hardware device. We draw to this "offscreen buffer" by making it the target of all our graphical operations. And when its time to show the completed scene, we copy the offscreen buffer into framebuffer in one big memory copy operation, known as a blit (see next section).
                                                                                                                                                                                                                                                                                                                 
           For the new_offscreen_buffer() function, we will allocate a screen-sized region of our address space. Normally, we would use the C Standard Library function malloc() to do this, but we are avoiding all library calls. So we will once again ask the OS for memory directly, by using the same mmap() system call we used in init_graphics(). This time, we will pass slightly different parameters. The memory should still be readable and writeable, but rather than MAP_SHARED, we wish to make a private, anonymous allocation. Anonymous means that we do not have a file to map into the region. Use MAP_PRIVATE|MAP_ANONYMOUS and a file descriptor of -1 to use mmap() just like malloc(). The return value (if successful) will be the address of an appropriately-sized (screen-sized) contiguous block of address space. Return that address back to the caller.
                                                                                                                                                                                                                                                                                                                 
                                                                                                                                                                                                                                                                                                                 
    //blit()
        1. A blit is simply a memory copy from our offscreen buffer to the framebuffer. If we were using the C Standard Lib, we would do this using memcpy(), but we must write it ourselves. So, use one or two for loops and copy every byte from the source offscreen buffer onto the framebuffer.
                                                                                                                                                                                                                                                                                                                 
                                                                                                                                                                                                                                                                                                                 
    //draw_pixel()
        1. This is the main drawing code, where the work is actually done. We want to set the pixel at coordinate (x,y) to the specified color. You will use the given coordinates to scale the base address of the memory-mapped framebuffer using pointer arithmetic. The image will be stored in row-major order, meaning that the first row starts at offset 0 and then that is followed by the second row of pixels, and so on.
                                                                                                                                                                                                                                                                                                                 
                                                                                                                                                                                                                                                                                                                 
    //draw_line()
        1. Using draw_pixel(), make a line from (x1,y1) to (x2,y2). Use Bresenghams Algorithm with only integer math. Find your favorite online source to do so (and please cite the source in a comment), but make sure your implementation works for all valid coordinates and slopes.
                                                                                                                                                                                                                                                                                                                 
                                                                                                                                                                                                                                                                                                                 
    //clear_screen()
        1. When we want to blank out our offscreen buffer or our framebuffer, we can just copy over every byte with the value zero. So, much like our blit(), loop over the image buffer parameter and set each byte to zero.

