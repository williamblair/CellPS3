/* Low Level screen display test
 * based on 
 * http://betteros.org/tut/graphics1.php#fbdev
 * 
 * 04/01/2018
 * */

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/* Linux frame buffer includes */
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <asm/ps3fb.h> // specific to ps3 cell sdk I assume

#define FB_NAME "/dev/fb0"

/* Maps 8bit rgb values to a uint32_t */
inline uint32_t RGB32(uint8_t r, uint8_t g, uint8_t b, struct fb_var_screeninfo *vinfo)
{
    return ((r << vinfo->red.offset) | (g << vinfo->green.offset) | (b << vinfo->blue.offset));
}

int main(void)
{
    /* return status of frame buffer open */
    int fb;
    
    /* Pointer to the frame buffer address */
    uint8_t *fbp;
    
    /* The size of the screen buffer in bytes */
    long screensize;
    
    /* screen information */
    struct fb_fix_screeninfo finfo;
    struct fb_var_screeninfo vinfo;
    
    /* Open the frame buffer */
    fb = open(FB_NAME, O_RDWR);
    
    /* Get screen info */
    ioctl(fb, FBIOGET_VSCREENINFO, &vinfo); // variable screen info
    ioctl(fb, FBIOGET_FSCREENINFO, &finfo); // fixed screen info
    
    /* Set variable display info */
    vinfo.grayscale = 0;
    vinfo.bits_per_pixel = 32;
    ioctl(fb, FBIOPUT_VSCREENINFO, &vinfo);
    ioctl(fb, FBIOGET_VSCREENINFO, &vinfo);
    
    /* Get the screen size 
     * yres_virtual - # of horizontal lines
     * line_length - length of each line in bytes */
    screensize = vinfo.yres_virtual * finfo.line_length;

    /* Map the buffer in memory */
    fbp = mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb, (off_t)0);
    
    /* Now we can start drawing to the screen */
    long x,y, location;
    uint32_t pixel = RGB32(0xFF,0x00,0xFF,&vinfo); // purple
    
    // fill the screen!
    for(x=0;x<vinfo.xres;x++)
    {
        for(y=0;y<vinfo.yres;y++)
        {
            /* Calculate the pixel location in the frame buffer */
            location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
                       (y+vinfo.yoffset) * finfo.line_length;
            
            /* Set the pixel color */
            *((uint32_t*)(fbp + location)) = pixel;
        }
    }
    
    sleep(5);
    
    return 0;
}

