/**
 * imgsize.c
 *
 * Copyright (c) 2011 Alexandru G. <alex@no-spam-gentle.ro>
 */

#include <stdio.h>
#include <stdlib.h>
//#include <stdint.h>
//#include <fcntl.h>
//#include <unistd.h>

#define VERSION "0.1.0"

void usage() {
  printf(
    "\n"
    " Usage: imgsize <path>\n"
    "\n");
  exit(1);
}

void GetImageSize(const char *path, int *x, int *y)
{    
    FILE *fd=fopen(path,"rb");
    if (fd < 0) {
        perror("open()");
        exit(1);
    }
    
    fseek(fd,0,SEEK_END); 
    long len=ftell(fd); 
    fseek(fd,0,SEEK_SET); 
    if (len<24) { 
        fclose(fd); 
    }
        
    unsigned char buf[24]; fread(buf,1,24,fd);
    
    // For JPEGs, we need to read the first 12 bytes of each chunk.
    // We'll read those 12 bytes at buf+2...buf+14, i.e. overwriting the existing buf.
    if (buf[0]==0xFF && buf[1]==0xD8 && buf[2]==0xFF && buf[3]==0xE0 && buf[6]=='J' && buf[7]=='F' && buf[8]=='I' && buf[9]=='F')
    { 
        long pos=2;
        while (buf[2]==0xFF)
        { 
            if (buf[3]==0xC0 || buf[3]==0xC1 || buf[3]==0xC2 || buf[3]==0xC3 || buf[3]==0xC9 || buf[3]==0xCA || buf[3]==0xCB) break;
            pos += 2+(buf[4]<<8)+buf[5];
            if (pos+12>len) break;
            fseek(fd,pos,SEEK_SET); fread(buf+2,1,12,fd);    
        }
    }
    
    fclose(fd);
    
    // JPEG: (first two bytes of buf are first two bytes of the jpeg file; rest of buf is the DCT frame
    if (buf[0]==0xFF && buf[1]==0xD8 && buf[2]==0xFF)
    {
        *y = (buf[7]<<8) + buf[8];
        *x = (buf[9]<<8) + buf[10];
    }
    
    // GIF: first three bytes say "GIF", next three give version number. Then dimensions
    if (buf[0]=='G' && buf[1]=='I' && buf[2]=='F')
    { 
        *x = buf[6] + (buf[7]<<8);
        *y = buf[8] + (buf[9]<<8);
    }
    
    // PNG: the first frame is by definition an IHDR frame, which gives dimensions
    if ( buf[0]==0x89 && buf[1]=='P' && buf[2]=='N' && buf[3]=='G' && buf[4]==0x0D && buf[5]==0x0A && buf[6]==0x1A && buf[7]==0x0A
        && buf[12]=='I' && buf[13]=='H' && buf[14]=='D' && buf[15]=='R')
    {
        *x = (buf[16]<<24) + (buf[17]<<16) + (buf[18]<<8) + (buf[19]<<0);
        *y = (buf[20]<<24) + (buf[21]<<16) + (buf[22]<<8) + (buf[23]<<0);
    }
    
}

int main(int argc, const char **argv)
{
    if (argc < 2) usage();
    
    int x,y;
    const char *path = argv[1];
    
    GetImageSize(path, &x, &y);
    printf("width: %dpx; height: %dpx;\n", x, y );
    
    return 0;
}
