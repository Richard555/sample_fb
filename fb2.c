#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>

int main(int argc, char* argv[])
{
  int fbfd = 0;
  struct fb_var_screeninfo vinfo_orig;
  struct fb_var_screeninfo vinfo;

  // Open the file for reading and writing
  fbfd = open("/dev/fb0", O_RDWR);
  if (!fbfd) {
    printf("Error: cannot open framebuffer device.\n");
    return(1);
  }
  printf("The framebuffer device was opened successfully.\n");

  // Get variable screen information
  if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
    printf("Error reading variable information.\n");
  }

  // Store for reset
  memcpy(&vinfo_orig, &vinfo, sizeof(struct fb_var_screeninfo));

  // Set variable info
  vinfo.xres = 320;
  vinfo.yres = 240;
  vinfo.xres_virtual = 320;
  vinfo.yres_virtual = 480; // double the physical height
  vinfo.bits_per_pixel = 8;
  if (ioctl(fbfd, FBIOPUT_VSCREENINFO, &vinfo)) {
    printf("Error setting variable information.\n");
  }

  long int screensize = vinfo.xres * vinfo.yres;
  long int bufsize = vinfo.xres_virtual * vinfo.yres_virtual;
  char *fbp = 0;
  int vs = 0;
  int y = 0;

  // Map fb to user mem 
  fbp = (char*)mmap(0, bufsize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);

  if (fbp == (void*)-1) {
    printf("Failed to mmap.\n");
  }
  else {
    // draw a blue rect at the upper half of screen
    memset(fbp, 1, screensize / 2);
    // draw a red rect at the lower half of screen
    memset(fbp + screensize / 2, 4, screensize / 2);
    // draw a green rect at the upper half of 2nd screen
    memset(fbp + screensize, 2, screensize / 2);
    // draw a green rect at the lower half of 2nd screen
    memset(fbp + screensize + screensize / 2, 2, screensize / 2);

    sleep(2);

    // change to show lower half
    if (ioctl(fbfd, FBIO_WAITFORVSYNC, 0)) {
      printf("VSync failed.\n");
    }
    vinfo.yoffset = vinfo.yres;
    if (ioctl(fbfd, FBIOPAN_DISPLAY, &vinfo)) {
      printf("Pan failed.\n");
    }

    // wait
    sleep(2);
  }

  // Cleanup
  munmap(fbp, screensize);
  if (ioctl(fbfd, FBIOPUT_VSCREENINFO, &vinfo_orig)) {
    printf("Error re-setting variable information.\n");
  }
  close(fbfd);

  return 0;
}
