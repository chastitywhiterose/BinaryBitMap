#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

/*basic input/output and memory management functions*/
#include "bbmio.h"
/*basic graphics functions*/
#include "bbmlib.h"
/*functions testing or changing colors*/
#include "bbm_palette.h"

int main(int argc, char* argv[])
{

 uint32_t *p=NULL,*p1,*p2; /*The pointers to the pixels*/
 uint32_t width=1280,height=720; /*The default size of the image.*/
 char filename[256];
 int square_size=32;

 int frame=1,framemax=30000;

  sprintf(filename,"./i/v/%08d.bmp",frame);
 /*load first frame to get dimensions*/
  BBM_Load_BMP(&p,&width,&height,filename);
  if(p==NULL){printf("Pointer is NULL\n"); return 1;} /*Must error check in case could not read file.*/

/*
BBM_Set_BPP(p,width,height,2);
bbm_palette_count(p,width,height);
return 0;
*/

 /*Then use the width and height from the last to make a new image of the same size.*/
 p1=BBM_malloc(width,height);
 chastity_checker(p1,width,height,square_size);

 /*Then make a third image of the same size.*/
 p2=BBM_malloc(width,height);

 while(frame<=framemax)
 {

  sprintf(filename,"./i/v/%08d.bmp",frame);
  BBM_Load_BMP(&p,&width,&height,filename);
  if(p==NULL){printf("Pointer is NULL\n"); break;}

  if(0) /*if this is nonzero, then XOR pixels from p and p1 together to make p2 image.*/
  {
   uint32_t x,y;
   y=0;
   while(y<height)
   {
    x=0;
    while(x<width)
    {
     p[x+y*width]=p[x+y*width]^p1[x+y*width];
     x++;
    }
    y++;
   }
  }


  BBM_Set_BPP(p,width,height,3);
  sprintf(filename,"./o/%08d.bmp",frame);
  BBM_Save_BMP(p,width,height,filename,24);


  /*BBM_roll_left(p1,width,height);*/

  frame++;
 }

 printf("frame==%d\n",--frame);

 if(p!=NULL){free(p);}
 if(p1!=NULL){free(p1);}
 if(p2!=NULL){free(p2);}


 return 0;
}



/*
This is a special program I wrote which involves editing video gameplay of Super Mario Bros. 1. 

The program is compiled like this.
gcc -Wall -ansi -pedantic smb1.c -o main && ./main

However, because this requires video frames as input, it is required to have them ready.
From the main bbm tree where this file is, I had a directory "./i/v" which means a directory v inside directory i.

So first I change directory:
cd ./i/v

Then run this command to get bmp frames. (Other formats are possible too with ffmpeg)
ffmpeg -i video.mp4 -f image2 %08d.bmp

Or use another supported ffmpeg format if you prefer.
https://ffmpeg.org/general.html#Image-Formats
However, doing so would require modification of my code to load the files. Currently BMP and PPM are the only mainstream full color formats I have written functions for.

Then get the audio.
ffmpeg -i video.mp4 audio.wav

The framerate of the video is 30. This will be important later when making a new video file.

Encode the frames without audio.
ffmpeg -r 30 -f image2 -i ./o/%08d.bmp ./o/video.mp4

Encode the frames with audio.
ffmpeg -i ./i/v/audio.wav -r 30 -f image2 -i ./o/%08d.bmp ./o/video.mp4
*/