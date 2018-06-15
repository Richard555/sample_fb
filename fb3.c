#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/time.h>


#define BYTE __u8
#define WORD __u16
#define DWORD __u32

int main(int argc, char* argv[])
{
  int fbfd = 0;
  struct fb_var_screeninfo vinfo_orig;
  struct fb_var_screeninfo vinfo;
  struct fb_fix_screeninfo finfo;
  int x = 0, y = 0, repeat=0, i=0, ret;
//  long int location = 0;
  long int offset = 0;
  char *pfb, *pSrc, *pDst = 0;
  int vs, length = 0;
  DWORD color, A,R,G,B;
        

  struct timeval t1, t2;
  struct timezone tz;
  float deltatime;
  float totaltime = 0.0f;
  float rate = 0;
  float totaldata=0;

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

  // Get fixed info
  if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
    printf("Error reading fixed information.\n");
    return 1;
  }

  printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);
  printf("line_length = %d\n", finfo.line_length);

  
  long int screensize = finfo.line_length * vinfo.yres;
  //long int bufsize = vinfo.xres_virtual * vinfo.yres_virtual;

  // Map fb to user mem 
  pfb = (char*)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);

  if (pfb == (void*)-1) {
    printf("Failed to mmap.\n");
    return -1;
  }
  
  //test memory data move 
  gettimeofday(&t1, &tz);
  for (repeat=0; repeat <1; repeat ++){
  pSrc=pfb;
  pDst=pfb + (350*finfo.line_length);
  length=1000*250*vinfo.bits_per_pixel/8;
  memcpy((void*)pDst, (void*)pSrc, length);
  }
  gettimeofday(&t2, &tz);
  deltatime = (float)(t2.tv_sec - t1.tv_sec + (t2.tv_usec - t1.tv_usec) * 1e-6);

  totaldata = repeat*1000*250*(vinfo.bits_per_pixel/8);
  rate = (totaldata / deltatime) / 1e6; /* MB/sec*/

  printf("Memory memcpy speed is      = %f MB/sec \n", rate );


  //Test memory write only

  gettimeofday ( &t1 , &tz );
    
  // Make a square that is 1000x500
  for (repeat=0;repeat<10;repeat++){
   for (y=0; y<500; y++){
    for (x = 0; x < 1000; x++) {

    offset = y*finfo.line_length + x*(vinfo.bits_per_pixel/8);

    //use any color you like
    A=(0x00            ) ;
    R=(0x7f + repeat<<4) ;
    G=(0x7f + repeat<<4) ;
    B=(0x7f + repeat<<4) ;
    
    if (vinfo.bits_per_pixel == 32)
    {
        color = A <<24 | (R<<16) | (G<<8) | (B) ;
        *(DWORD*)(pfb+offset)=color;
    }
    
    if (vinfo.bits_per_pixel == 16)
    {
        color = (0x7f <<11) | (0x7f <<6) | (0x7f) ;
        *(WORD*)(pfb+offset)=color;
    }
   }
  }
  }

  gettimeofday(&t2, &tz);
  deltatime = (float)(t2.tv_sec - t1.tv_sec + (t2.tv_usec - t1.tv_usec) * 1e-6);

  totaldata = repeat*1000*500*(vinfo.bits_per_pixel/8);
  rate = (totaldata / deltatime) / 1e6; /* MB/sec*/

  printf("Memory write pixel speed is = %f MB/sec \n", rate );


  // Cleanup
  munmap(pfb, screensize);

//  if (ioctl(fbfd, FBIOPUT_VSCREENINFO, &vinfo_orig)) {
//    printf("Error re-setting variable information.\n");
//  }

  // test vsync
  int arg = 0 ; 
  
  gettimeofday(&t1 , &tz );

  for (i=0;i<60;i++){
    	ret=ioctl(fbfd, FBIO_WAITFORVSYNC, &arg);
  }

  gettimeofday(&t2, &tz);
  deltatime = (float)(t2.tv_sec - t1.tv_sec + (t2.tv_usec - t1.tv_usec) * 1e-6);
  
  printf ("Total time for testing 60 Vsyncs is %f sec\n", deltatime);
  printf ("Return value of IOCTL is %d \n", ret);

  close(fbfd);

  return 0;
}
