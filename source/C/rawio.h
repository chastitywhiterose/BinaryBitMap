/*
rawio.h

This file exists for reading and writing the files of raw pixel data that can be converted by imagemagick into other formats.
There are three types of reading and writing operations.

Monochrome Image Facts

Monochrome or Bilevel images contain only two colors, usually black and white.
A single bit is usually used to represent a pixel. This means a single byte can store 8 pixels. For this reason, two color images are the smallest whether compressed or uncompressed.

However there are particular differences when it comes to conventions about how the bits are stored and what they represent. Some like PBM use the value 1 to be black and 0 to be white. Others can do it either way. TIFF tag 262 determines this in a TIFF file. BMP files use a 2 color palette which allows either 0 or 1 to equal any color, not just black and white. Personally I think 0 should be used for black and 1 for white. It makes sense to me but this is also an arbitrary choice which format creators disagree on.


PBM and BMP use the high bit to be the leftmost pixel. Others like XBM and the raw monochrome data that Imagemagick reads use the low bit instead. There are two ways of going about this. One is to use either right shifting or left shifting as you place bits into a byte, the other is to simply use one method and then reverse the bits in a byte before writing it to the file. TIFF has tag 266 that determines the rule of which order they are in.


*/


/*
 A function to reverse the bits in a byte. Some monochrome formats like PBM expect the highest bit to be the first pixel. Others like the XBM format as well as the monochrome data imagemagick produces expect the low bit to be the first. This function can easily swap between the two methods. It's used to reverse the bytes in the following functions which normally extracts the bits out of the same order as the functions that load bits from PBM and BMP files.


*/
uint32_t BitReverse8(uint32_t a)
{
 uint32_t x=0,b=0;
 while(x<8) /*Loop which reverses the bits!*/
 {
  b<<=1;
  b|=a&1;
  a>>=1;
  x++;
 }
 return b;
}


/*
Saving monochrome data in format imagemagick expects. This is a copy of the same algorithm as was used in my BBM_Save_BBM_Pixels from bbmio.h . However it additionally reverses the bits and inverts them just before writing the byte with fputc.
*/
void raw_save_mono(uint32_t *p,uint32_t width,uint32_t height,const char *filename)
{
 uint32_t x,y,pixel,r,g,b,gray,bitcount,bits,bpp=1;
 FILE* fp;
 fp=fopen(filename,"wb+");
 if(fp==NULL){printf("Failed to create file \"%s\".\n",filename); return;}
 else{printf("File \"%s\" opened for writing.\n",filename);}

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
    bits^=255;       /*imagemagick's mono format is like pbm. 0=white,1=black*/
    bits=BitReverse8(bits); /*reverse bits order*/
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
    bits^=255;
    bits=BitReverse8(bits);
    fputc(bits,fp);
    bitcount=0;
   }
  }
  y++;
 }

 fclose(fp);
}


/*
An alternate function which does the same thing but does not call the bit reversing function. Instead the bits variable is right shifted instead of left shifted. It's still bitwise ORed with the gray variable but the gray variable does not need to be right shifted because that was only done for the purposes of Oring it with the lower part of the byte. This means that ALL bitshifts are right shifts. It's theoretically faster but has not been tested as much. So far the results seem to work for the purpose of 1 bit per pixel images. The algorithm originally was built with the assumption that 2,4, or 8 bits grayscale could be used by modifying the bpp variable. That's why the bpp variable appears instead of the constant 1. However in that case an actual raw grayscale format and functions would make more sense.
*/
void raw_save_mono1(uint32_t *p,uint32_t width,uint32_t height,const char *filename)
{
 uint32_t x,y,pixel,r,g,b,gray,bitcount,bits,bpp=1;
 FILE* fp;
 fp=fopen(filename,"wb+");
 if(fp==NULL){printf("Failed to create file \"%s\".\n",filename); return;}
 else{printf("File \"%s\" opened for writing.\n",filename);}

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
   bits>>=bpp;
   bits|=gray;
   bitcount+=bpp;
   x++;
   while(bitcount>=8)
   {
    bits^=255; 
    fputc(bits,fp);
    bitcount-=8;
   }
  }

  /*If width is not a multiple of 8 pad the bits to a full byte*/
  while(bitcount!=0)
  {
   bits>>=1;
   bitcount++;
   if(bitcount==8)
   {
    bits^=255;
    fputc(bits,fp);
    bitcount=0;
   }
  }
  y++;
 }

 fclose(fp);
}


/*
A function that loads the monochrome data output by imagemagick. You are required to know the width and height of the image to pass as arguments because raw images don't contain anything except the pixel data. It's also required that the pointer is already allocated with enough memory for width*height pixels.
*/
void raw_load_mono(uint32_t *p,uint32_t width,uint32_t height,const char *filename)
{
 uint32_t x,y,pixel,x2,c,bitcount,bits,bpp=1;
 FILE* fp;
 fp=fopen(filename,"rb");
 printf("This function loads raw monochrome data from a file into memory.\n");
 if(fp==NULL){printf("Failed to read file \"%s\": Doesn't exist.\n",filename); p=NULL; return;}
 else{printf("File \"%s\" opened for reading.\n",filename);}

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
    c^=255;        /*imagemagick's mono format is like pbm. 0=white,1=black*/
    c=BitReverse8(c); /*reverse bits order*/
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

 fclose(fp);
}


/*
Saving 8 bit grayscale in format imagemagick expects.
*/
void raw_save_gray(uint32_t *p,uint32_t width,uint32_t height,const char *filename)
{
 uint32_t x,y,pixel,r,g,b,gray;
 FILE* fp;
 fp=fopen(filename,"wb+");
 if(fp==NULL){printf("Failed to create file \"%s\".\n",filename); return;}
 else{printf("File \"%s\" opened for writing.\n",filename);}

 y=0;
 while(y<height)
 {
  x=0;
  while(x<width)
  {
   pixel=p[x+y*width];
   r=(pixel&0xFF0000)>>16;
   g=(pixel&0x00FF00)>>8;
   b=(pixel&0x0000FF);
   gray=(r+g+b)/3;
   fputc(gray,fp);
   x++;
  }
  y++;
 }

 fclose(fp);
}


/*
This function loads the 8 bpp grayscale data where each byte is ranged 0(black) to 255(white).
Because it assumes each pixel is one byte grayscale, it's faster than using the same algorithm as the monochrome function even though it would have worked correctly given bpp variable of 8. The goal is fastest loading.
*/
void raw_load_gray(uint32_t *p,uint32_t width,uint32_t height,const char *filename)
{
 uint32_t x,y,pixel,c;
 FILE* fp;
 fp=fopen(filename,"rb");
 printf("This function loads raw 8 bit grayscale data from a file into memory.\n");
 if(fp==NULL){printf("Failed to read file \"%s\": Doesn't exist.\n",filename); p=NULL; return;}
 else{printf("File \"%s\" opened for reading.\n",filename);}

 y=0;
 while(y<height)
 {
  x=0;
  while(x<width)
  {
   pixel=0;
   c=fgetc(fp);
   if(feof(fp)){printf("Error: End of file reached.\n");}
   pixel|=c;
   pixel|=c<<8;
   pixel|=c<<16;
   p[x+y*width]=pixel;
   x++;
  }
  y++;
 }

 fclose(fp);
}


/*
Saving rgb in format imagemagick expects.
*/
void raw_save_rgb(uint32_t *p,uint32_t width,uint32_t height,const char *filename)
{
 uint32_t x,y,pixel,r,g,b;
 FILE* fp;
 fp=fopen(filename,"wb+");
 if(fp==NULL){printf("Failed to create file \"%s\".\n",filename); return;}
 else{printf("File \"%s\" opened for writing.\n",filename);}

  y=0;
  while(y<height)
  {
   x=0;
   while(x<width)
   {
    pixel=p[x+y*width];
    r=(pixel&0xFF0000)>>16;
    g=(pixel&0x00FF00)>>8;
    b=(pixel&0x0000FF);
    fputc(r,fp);
    fputc(g,fp);
    fputc(b,fp);
    x++;
   }
   y++;
  }

 fclose(fp);
}


/*
load raw rgb
*/
void raw_load_rgb(uint32_t *p,uint32_t width,uint32_t height,const char *filename)
{
 uint32_t x,y,pixel,r,g,b;
 FILE* fp;
 fp=fopen(filename,"rb");
 printf("This function loads raw Red Green Blue data from a file into memory.\n");
 if(fp==NULL){printf("Failed to read file \"%s\": Doesn't exist.\n",filename); p=NULL; return;}
 else{printf("File \"%s\" opened for reading.\n",filename);}

 y=0;
 while(y<height)
 {
  x=0;
  while(x<width)
  {
   pixel=0;
   r=fgetc(fp);
   g=fgetc(fp);
   b=fgetc(fp);
   pixel|=r<<16;
   pixel|=g<<8;
   pixel|=b;
   p[x+y*width]=pixel;
   x++;
  }
  y++;
 }

 fclose(fp);
}



/*
Mono conversion function using imagemagick. If it is not installed it will most likely print a horrible error.
This one takes the passed width and height arguments to form the command required.

You can also use this command to see the supported formats:
magick convert -list format
*/
void raw_mono_magick(uint32_t width,uint32_t height,const char* infile,const char* outfile)
{
 char command[256];
 sprintf(command,"magick convert -size %ux%u %s %s",width,height,infile,outfile);
 printf("%s\n",command);
 system(command);
}



/*
To convert a bmp image to raw pixel data.

magick convert ./o/bbm.bmp ./o/bbm.mono
magick convert ./o/bbm.bmp ./o/bbm.gray
magick convert ./o/bbm.bmp ./o/bbm.rgb

Convert to PNG

magick convert -size 512x512 ./o/bbm.mono ./o/bbm1.png
magick convert -depth 8 -size 512x512 ./o/bbm.gray ./o/bbm2.png
magick convert -depth 8 -size 512x512 ./o/bbm.rgb ./o/bbm3.png


magick convert -size 640x1136 ./i/violinist.mono ./o/bbm.png
magick convert -depth 8 -size 640x1136 ./i/violinist.gray ./o/bbm.png

soon I may have ffmpeg command examples after I learn more about how it works.
https://ffmpeg.org/general.html#Image-Formats
https://ffmpeg.org/ffmpeg-formats.html#rawvideo
https://github.com/stoyanovgeorge/ffmpeg/wiki/Encode-Raw-Video
*/