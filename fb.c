#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>

int main(int argc, char* argv[])
{
    int fbfd = 0;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    long int screensize = 0;
    char *fbp = 0;
    int x = 0, y = 0;
    long int location = 0;

    // Open the file for reading and writing
    fbfd = open("/dev/fb0", O_RDWR);//���}framebuffer�A���o��fd
    if (!fbfd) {
        printf("Error: cannot open framebuffer device.\n");
        return 1;
    }
    printf("The framebuffer device was opened successfully.\n");

    // ���o��d��fix�ݩ�
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
        printf("Error reading fixed information.\n");
        return 1;
    }

    // ���o��d��var�ݩ�
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
        printf("Error reading variable information.\n");
        return 1;
    }

    printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

    // ��X���screen�һݭn��size�A�Hbyte�����A�H�K�y��N�O����map��userspace
    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

    // �o�˴Nmap��userspace�F
    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED,
                       fbfd, 0);
    if ((int)fbp == -1) {
        printf("Error: failed to map framebuffer device to memory.\n");
        return 1;
    }
    printf("The framebuffer device was mapped to memory successfully.\n");

    x = 100; y = 100;       // screen���W����0,0�A�b�y�Ь�(100,100)���a��}�l�e

    // �e�@��200x200��pixel�������
    for (y = 100; y < 300; y++)
        for (x = 100; x < 300; x++) {

            location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
                       (y+vinfo.yoffset) * finfo.line_length;

            if (vinfo.bits_per_pixel == 32) {

               //�H�U�O��o��pixel���ݩʡA�]�ARGB���ȡA�H�γz�����{��
                *(fbp + location) = 100;        // Some blue
                *(fbp + location + 1) = 15+(x-100)/2;     // A little green
                *(fbp + location + 2) = 200-(y-100)/5;    // A lot of red
                *(fbp + location + 3) = 0;      // No transparency
            }
        }
    munmap(fbp, screensize); //�k�ٰO����
    close(fbfd);//�O�o�n����framebuffer��fd
    return 0;
}