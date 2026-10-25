/*Binary Bit Map Input and Output*/

/*
This header file is all about the functions that read my own BBM format and a few other formats.
Because the format is my own invention and no other programs can convert between it and others(unless another programmer reads the manual I wrote for it), I have the challenge of writing a series of functions that can load files into an array of pixels and also write it to any format for which I have written a function for.

The main.h file was just getting too large for me to scroll through so I am organizing the relevant functions into here.
*/

/*The following typedefs were originally done by SDL_stdinc.h and are here because I happen to like referring to these types by theses names.
As long as I have the line
#include <stdint.h>
before this then the type rename works just fine.
*/

/*typedef uint8_t Uint8;*/
/*typedef uint16_t Uint16;*/
/* typedef uint32_t Uint32; */
/*typedef uint64_t Uint64;*/

/*You can change black and white to other colors.*/
uint32_t u32bw[]={0x000000,0xFFFFFF};

/*
 Allocates memory for the pixels which should be 4 bytes/32 bits per pixel. Uses standard library function malloc.
 uint32_t is a 32 bit unsigned integer type. This is why stdint.h is always included.
 I never need more than 32 bits and any more would waste memory.
*/
uint32_t* BBM_malloc(uint32_t width,uint32_t height)
{
 uint32_t *pointer;
 int length=width*height;
 pointer=(uint32_t*)malloc(length*sizeof(*pointer));
 if(pointer==NULL){printf("Error: malloc failed,\n");}
 return pointer;
}

/*
frees the memory the pointer points to, but only if the pointer is not already NULL.
*/
void BBM_free(uint32_t *pointer)
{
 if(pointer!=NULL){free(pointer);pointer=NULL;}
}


/*
A quick dirty replacement for never needing to use fwrite to write my integers! However the catch is that it only works for little endian. For this reason I choose little endian as the format the binary integers will take in my own image formats I am creating. It writes integer i to file pointer fp using count bytes. Most PCs these days can't actually do more than 32 or 64 bits.

This function was necessary so that my code won't completely fail if I do programming on a different CPU in which case fwrite would spit out the big endian byte order. Besides it wasn't too hard to write and requires only 3 arguments instead of 4.
*/
void fputint(unsigned long int i,FILE *fp,int count)
{
 while(count>0)
 {
  fputc(i,fp);
  i>>=8;
  count--;
 }
}

/*The reverse process gets an integer of the specified number of bytes in little endian format.*/
unsigned long int fgetint(FILE *fp,int count)
{
 unsigned long int i=0,c,x;
 
 x=0;
 while(count>0)
 {
  c=fgetc(fp);
  i+=c<<=x;
  x+=8;
  count--;
 }

 return i;
}

/*
 Original Binary Bit Map header. Contains only the width and height of the image.
 Small but leaves little information which can be used to identify the file.
*/
void BBM_Save_BBM_Header_Version_0(uint32_t *p,uint32_t width,uint32_t height,FILE* fp)
{
 fputint(width,fp,2);
 fputint(height,fp,2);
}

/*
 New Binary Bit Map header. Contains the following data;
 width
 height
 bits per pixel (always 1 in the case of BBM)
 pointer to pixel data
 optional comment
 
 Aside from the comment, which can be any number of bytes between the header and the start of the pixel data,
 the other 4 values are exactly 4 bytes. The pointer is used for skipping the comment for the function which
 will read this format.
*/
void BBM_Save_BBM_Header_Version_1(uint32_t *p,uint32_t width,uint32_t height,FILE* fp,int bpp)
{
 int data=0;
 fputint(width,fp,4);
 fputint(height,fp,4);
 fputint(bpp,fp,4);
 fputint(data,fp,4);
 fprintf(fp,"Binary Bit Map  ");
 data=ftell(fp);
 fseek(fp,0xC,SEEK_SET);
 fputint(data,fp,4);
 fseek(fp,data,SEEK_SET);
}


/*
A function which writes the pixels for my BBM format. 1 bit per pixel. 0 is black and 1 is white. This is the official bit packing function of the BBM project. It does scanline padding exactly the way that WBMP and PBM do. This was done for reasons of compatibility. However there is a trick. This function works correctly if you call it with a bpp of 2,4, or 8. The routine I coded packs the pixels perfectly and there was no reason to restrict it to only 1 bit per pixel, even though that was the only reason I wrote the function in the first place.
*/
void BBM_Save_BBM_Pixels(uint32_t *p,uint32_t width,uint32_t height,FILE* fp,int bpp)
{
 uint32_t x,y,pixel,r,g,b,gray,bitcount,bits;

 y=0;
 while(y<height)
 {
  bitcount=0;
  bits=0;
  x=0;
  while(x<width)
  {
   pixel=p[x+y*width];
   r=(pixel&0xFF0000)>>16;
   g=(pixel&0x00FF00)>>8;
   b=(pixel&0x0000FF);
   gray=(r+g+b)/3;
   gray>>=8-bpp;
   bits<<=bpp;
   bits|=gray;
   bitcount+=bpp;
   x++;
   while(bitcount>=8)
   {
    fputc(bits,fp);
    bitcount-=8;
   }
  }

  /*If width is not a multiple of 8 pad the bits to a full byte*/
  while(bitcount!=0)
  {
   bits<<=1;
   bitcount++;
   if(bitcount==8)
   {
    fputc(bits,fp);
    bitcount=0;
   }
  }
  y++;
 }

}




/*
Saves to BBM: Binary Bit Map

I need to write a proper updated specification for this format when I have time.
*/
void BBM_Save_BBM(uint32_t *p,int width,int height,const char *filename,int bpp)
{
 FILE* fp;
 fp=fopen(filename,"wb+");
 if(fp==NULL){printf("Failed to create file \"%s\".\n",filename); return;}
 /*else{printf("File \"%s\" opened for writing.\n",filename);}*/
 
 BBM_Save_BBM_Header_Version_1(p,width,height,fp,bpp);

 BBM_Save_BBM_Pixels(p,width,height,fp,bpp);
 
 fclose(fp);
}


/*
 Function which loads the pixels of the BBM format according to exactly how they would have been written previously.
 This is to be called only after the file has been opened and the file pointer is at the location where the pixels begin. It
 works correctly on bpp 1,2,4, or 8 if they were packed accurately with the BBM_SaveBBM function. The pixels are expected to  be packed into a byte with extra fill bits at the end of the row if width is not a multiple of 8.
*/

void BBM_LoadBBM_Pixels(uint32_t *p,uint32_t width,uint32_t height,FILE* fp,uint32_t bpp)
{
 uint32_t x,y,pixel,x2,c,bitcount,bits;

 y=0;
 while(y<height)
 {
  bitcount=0;
  x=0;
  while(x<width)
  {
   if(bitcount%8==0)
   {
    c=fgetc(fp);
    if(feof(fp))
    {
     printf("Error: End of file reached.\n");
     free(p); p=NULL; return;
    }
   }
   
   bits=c >> (8-bpp);
   c<<=bpp;
   c&=255;
   bitcount+=bpp;

   /*convert gray into a 24 bit RGB equivalent.*/
   pixel=0;
   x2=0;
   while(x2<24)
   {
    pixel<<=bpp;
    pixel|=bits;
    x2+=bpp;
   }

    p[x+y*width]=pixel;
    x++;
   }
   y++;
  }

}

/*
 This function calls the relevant functions to load version 1 of my BBM format. It gets the width, height, bits per pixel, and pointer where the image data starts. 
*/

void BBM_LoadBBM(uint32_t **p,uint32_t *width,uint32_t *height,const char *filename)
{
 int bpp,data;
 FILE* fp;
 fp=fopen(filename,"rb");
 printf("This function loads a BBM file into memory.\n");
 if(fp==NULL){printf("Failed to read file \"%s\": Doesn't exist.\n",filename); *p=NULL; return;}
 else{printf("File \"%s\" opened for reading.\n",filename);}

 *width=fgetint(fp,4);
 *height=fgetint(fp,4);
 bpp=fgetint(fp,4);
 data=fgetint(fp,4);

 printf("Width=%d\n",*width);
 printf("Height=%d\n",*height);
 printf("Bits Per Pixel=%d\n",bpp);
 printf("Pixel Data Starts at address=%d\n",data);

 fseek(fp,data,SEEK_SET);

 if(*p!=NULL)
 {
  printf("Pointer is not NULL. Will free memory first\n"); free(*p); *p=NULL;
 }

 *p=BBM_malloc(*width,*height);

 BBM_LoadBBM_Pixels(*p,*width,*height,fp,bpp);

 fclose(fp);
}

/*Include the header for my Binary Gray Map*/
#include "bgmio.h"

/*Include the header for my Binary Pixel Map*/
#include "bpmio.h"

/*Include the header for my Text Bit Map*/
#include "tbmio.h"

/*A function to turn any RGB pixel color into either black or white so it can be in a monochrome image.*/
uint32_t Cast_BW(uint32_t pixel)
{
 unsigned int x=0,r=0,g=0,b=0,gray=0;

 r=(pixel&0xFF0000)>>16;
 g=(pixel&0x00FF00)>>8;
 b=(pixel&0x0000FF);
 gray=(r+g+b)/3;
 x=gray>>7;

/*
 printf("Red=%d\n",r);
 printf("Green=%d\n",g);
 printf("Blue=%d\n",b);
 printf("Gray=%d\n",gray);
 printf("x=%d\n",x);
*/

 return x;
}

/*A function to turn any RGB pixel color into a 0 to 255 shade of gray for use in making grayscale image.*/
int Cast_Gray(uint32_t pixel)
{
 int r=0,g=0,b=0,gray=0;
 r=(pixel&0xFF0000)>>16;
 g=(pixel&0x00FF00)>>8;
 b=(pixel&0x0000FF);
 gray=(r+g+b)/3;
 return gray;
}



/*Include the header for the BMP format*/
#include "bbm_bmp.h"

#include "bbm_xbm.h"
#include "bbm_xpm.h"
#include "bbm_wbmp.h"

/*
 Older Header for NetPBM formats.
 This one can read and write PBM, PGM, and PPM files, but assumes maxval of 255.
*/
#include "bbm_pbm.h"

/*Newer Header for NetPBM formats.
*/
#include "pnmio.h"

/*Header for new PAM format of the NetPBM series.*/
#include "pamio.h"

/*TIFF header*/
#include "bbm_tif.h"

/*Include the header for the PNG format*/
#include "bbm_png.h"

















/*
These next two functions will be the last in this file. This is because they are responsible for loading and saving of files to different formats based on extension.
*/

/*This function loads a file or gives a message about it.*/
void BBM_Load(uint32_t **p,uint32_t *width,uint32_t *height,const char *filename)
{
 char *s=(char*)filename,*s1=s;
 printf("BBM_Load()\n");
 if(*p!=NULL)
 {
  printf("Pointer is not NULL. Will free memory first\n"); free(*p); *p=NULL;
 }
 printf("Identifying type of file: %s\n",s);
 while(*s1!=0)
 {
  if(*s1=='.'){s=s1;}
  s1++;
 }
 if(*s==0)
 {
  printf("File has no extension.\n");
  printf("Will assume BBM type.\n");
  BBM_LoadBBM(p,width,height,filename);
 }
 else
 {
  printf("File extension: %s\n",s);
  if(!strcmp(s,".bbm")){printf("Binary Bit Map\n"); BBM_LoadBBM(p,width,height,filename);}
  if(!strcmp(s,".bgm")){printf("Binary Gray Map\n"); BBM_LoadBGM(p,width,height,filename);}
  if(!strcmp(s,".bpm")){printf("Binary Pixel Map\n"); BBM_LoadBPM(p,width,height,filename);}
  if(!strcmp(s,".pbm")){printf("Portable Bit Map\n"); BBM_LoadPBM(p,width,height,filename);}
  if(!strcmp(s,".wbmp")){printf("Wireless Bit Map\n"); BBM_LoadWBMP(p,width,height,filename);}
  if(!strcmp(s,".pgm")){printf("Portable Gray Map\n"); BBM_LoadPGM(p,width,height,filename);}
  if(!strcmp(s,".ppm")){printf("Portable Pixel Map\n"); BBM_LoadPPM(p,width,height,filename);}
  if(!strcmp(s,".xbm")){printf("No Reader For: ");printf("X Window System Bitmap\n");}
  if(!strcmp(s,".bmp")){printf("Limited Reader For: ");printf("Microsoft Bitmap\n");BBM_Load_BMP(p,width,height,filename);}
  if(!strcmp(s,".png")){printf("No Reader For: ");printf("Portable Network Graphics\n");}
  if(!strcmp(s,".gif")){printf("No Reader For: ");printf("Graphics Interchange Format\n");}
  if(!strcmp(s,".tif")){printf("No Reader For: ");printf("Tagged Image File Format\n");}
 }
}

/*
A function which calls the external magick program from imagemagick. If it is not installed it will most likely print a horrible error.

You can also use this command to see the supported formats:
magick convert -list format
*/
void magick(const char* infile,const char* outfile)
{
 char command[256];
 sprintf(command,"magick convert %s %s",infile,outfile);
 printf("%s\n",command);
 system(command);
}

void gm(const char* infile,const char* outfile)
{
 char command[256];
 sprintf(command,"gm convert %s %s",infile,outfile);
 printf("%s\n",command);
 system(command);
}

/*
Of course gm from graphicsmagick can also be used.

gm convert -list format
*/


