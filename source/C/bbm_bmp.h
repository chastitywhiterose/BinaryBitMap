/*
The functions for saving to a Windows bitmap file were moved here because they are extremely large!
A fine example of the complexity of the format. The functions can make a valid BMP file that Facebook accepts but they have a very complex header that I spent weeks learning how to write effectively. However it is these functions upon which the rest of the library is built because they allow me to see what they look like without having to use SDL, Allegro, or another graphics library to display. Every image editor/viewer supports BMP files although I personally think it's a really inefficient format.

It's well documented on Wikipedia however.
https://en.wikipedia.org/wiki/BMP_file_format
*/




/*
Saves to BMP format but monochrome!

This function uses the 12 byte DIB header compatible with Windows 2.0 and later. This format is rarely used but can be made using imagemagick with this command from my pbm file.

magick convert -monochrome checker.pbm checker-im.bmp2

I used that to make a reference file when coding this function.

This function has also been updated with my fputint function I wrote to be slightly more portable and not depend on the endianness of the host machine it compiles on.
*/

void BBM_SaveBMP_BW(uint32_t *p,int width,int height,const char* filename)
{

 int x,y,data,bpp=1;
 FILE* fp;
 fp=fopen(filename,"wb+");
 if(fp==NULL){printf("Failed to create file \"%s\".\n",filename); return;}
 else{/*printf("File \"%s\" opened.\n",filename);*/}

 fputint(0x4D42,fp,0x1A); /*26 bytes because 14 file header + 12 dib header*/

 /*Set up the pallete*/
 fputint(u32bw[0],fp,3);
 fputint(u32bw[1],fp,3);

 /*fprintf(fp,"Chastity Checker");*/
 data=ftell(fp);
 /*printf("Pixels Start at file address: %d\n",data);*/

 y=height;
 while(y>0)
 {
  int bitcount=0;
  int c=0;
  y--;
  x=0;
  while(x<width)
  {
   int pixel=p[x+y*width];
   uint32_t bit=Cast_BW(pixel);

   c<<=1;
   c|=bit;
   bitcount++;
   x++;
   if(bitcount%8==0)
   {
    fputc(c,fp);
   }
  }

  /*This loop fixes things when the image is not a multiple of 32 bits in width.*/
  while(bitcount%32!=0)
  {
   c<<=1;
   bitcount++;
   if(bitcount%8==0)
   {
    fputc(c,fp);
   }
  }
 }

 /*start of header fill code*/

 x=ftell(fp); /*get the size of the file now that it has been written.*/
 fseek(fp,2,SEEK_SET); /*seek back to offset 2*/
 fputint(x,fp,4);

 fseek(fp,0xA,SEEK_SET); /*seek back to offset A*/
 fputint(data,fp,4); /*Tell it where the pixels start!*/

 fseek(fp,0xE,SEEK_SET); /*seek back to location for dib header size*/
 fputint(12,fp,4); /*The size of this header (12 bytes)*/

 fseek(fp,0x12,SEEK_SET); /*Seek to width and height location*/
 fputint(width,fp,2); /*The bitmap width in pixels (unsigned 16-bit)*/
 fputint(height,fp,2); /*The bitmap height in pixels (unsigned 16-bit)*/

 fseek(fp,0x16,SEEK_SET);
 fputint(1,fp,2); /*The number of color planes, must be 1*/

 fseek(fp,0x18,SEEK_SET); /*Bits Per Pixel location*/
 fputint(bpp,fp,2);

 /*end of header fill code*/

 fclose(fp);

 /*printf("Saved to file: %s\n",filename);*/

}


/*
A function specifically designed for making 3 bits per pixel color. BMP files support 4 bits per pixel indexed images. So in this case 3 bits are actually used in each half byte. 
*/

void BBM_Save_BMP_RGB_3(uint32_t *p,int width,int height,const char* filename)
{
 int x,y,data,bpp=3,bits,gray,bitcount,color,pixel,red,green,blue,palette_length=8;
 FILE* fp;
 fp=fopen(filename,"wb+");
 if(fp==NULL){printf("Failed to create file \"%s\".\n",filename); return;}
 else{/*printf("File \"%s\" opened.\n",filename);*/}

 fputint(0x4D42,fp,0x1A); /*26 bytes because 14 file header + 12 dib header*/

 /*Set up the pallete*/

  bpp/=3;
  gray=(1<<bpp)-1;

  x=0;
  while(x<palette_length)
  {
   color=x;
   blue=color&gray;color>>=bpp;
   green=color&gray;color>>=bpp;
   red=color&gray;color>>=bpp;

    red<<=16;
    green<<=8;

    bitcount=0;
    while(bitcount<8)
    {
     color<<=bpp;
     color|=red;
     color|=green;
     color|=blue;
     bitcount+=bpp;
    }

   fputint(color,fp,3);

   x++;
  }

 /*fprintf(fp,"Chastity Checker");*/
 data=ftell(fp);
 /*printf("Pixels Start at file address: %d\n",data);*/

 y=height;
 while(y>0)
 {
  y--;
  bits=0;
  x=0;
  while(x<width)
  {


   /*First, get the pixel and extract each color component.*/
    pixel=p[x+y*width];
    red=(pixel&0xFF0000)>>16;
    green=(pixel&0x00FF00)>>8;
    blue=(pixel&0x0000FF);

    /*right shift each component by the bpp to eliminate extra bits.*/
    red>>=8-bpp;
    green>>=8-bpp;
    blue>>=8-bpp;

    /*next left shift red and green back to their proper position as they would be in 3 bits rgb. blue doesn't need to be shifted.*/
    red<<=2;
    green<<=1;

    pixel=red|green|blue;


    bits<<=4;
    bits|=pixel;
    bitcount+=4;

   if(bitcount%8==0)
   {
    fputc(bits,fp);
   }

   x++;
  }

  /*This loop fixes things when the image is not a multiple of 32 bits in width.*/
  while(bitcount%32!=0)
  {
   bits<<=4;
   bitcount+=4;
   if(bitcount%8==0)
   {
    fputc(bits,fp);
   }
  }


 }

 /*start of header fill code*/

 x=ftell(fp); /*get the size of the file now that it has been written.*/
 fseek(fp,2,SEEK_SET); /*seek back to offset 2*/
 fputint(x,fp,4);

 fseek(fp,0xA,SEEK_SET); /*seek back to offset A*/
 fputint(data,fp,4); /*Tell it where the pixels start!*/

 fseek(fp,0xE,SEEK_SET); /*seek back to location for dib header size*/
 fputint(12,fp,4); /*The size of this header (12 bytes)*/

 fseek(fp,0x12,SEEK_SET); /*Seek to width and height location*/
 fputint(width,fp,2); /*The bitmap width in pixels (unsigned 16-bit)*/
 fputint(height,fp,2); /*The bitmap height in pixels (unsigned 16-bit)*/

 fseek(fp,0x16,SEEK_SET);
 fputint(1,fp,2); /*The number of color planes, must be 1*/

 fseek(fp,0x18,SEEK_SET); /*Bits Per Pixel location*/
 fputint(4,fp,2); /*4 according to the Microsoft API even though only 3 bits are used in this function.*/

 /*end of header fill code*/

 fclose(fp);

 /*printf("Saved to file: %s\n",filename);*/

}



/*
A function specifically designed for making 6 bits per pixel color.
*/

void BBM_Save_BMP_RGB_6(uint32_t *p,int width,int height,const char* filename)
{
 int x,y,data,bpp=6,gray,bitcount,color,pixel,red,green,blue,palette_length=64;
 FILE* fp;
 fp=fopen(filename,"wb+");
 if(fp==NULL){printf("Failed to create file \"%s\".\n",filename); return;}
 else{/*printf("File \"%s\" opened.\n",filename);*/}

 fputint(0x4D42,fp,0x1A); /*26 bytes because 14 file header + 12 dib header*/

 /*Set up the pallete*/

  bpp/=3;
  gray=(1<<bpp)-1;

  /*printf("graymask == %d\n",gray);*/

  x=0;
  while(x<palette_length)
  {
   color=x;
   blue=color&gray;color>>=bpp;
   green=color&gray;color>>=bpp;
   red=color&gray;color>>=bpp;

    red<<=16;
    green<<=8;

    bitcount=0;
    while(bitcount<8)
    {
     color<<=bpp;
     color|=red;
     color|=green;
     color|=blue;
     bitcount+=bpp;
    }

   fputint(color,fp,3);

   x++;
  }

 /*fprintf(fp,"Chastity Checker");*/
 data=ftell(fp);
 /*printf("Pixels Start at file address: %d\n",data);*/

 y=height;
 while(y>0)
 {
  bitcount=0;
  y--;
  x=0;
  while(x<width)
  {


   /*First, get the pixel and extract each color component.*/
    pixel=p[x+y*width];
    red=(pixel&0xFF0000)>>16;
    green=(pixel&0x00FF00)>>8;
    blue=(pixel&0x0000FF);

    /*right shift each component by the bpp to eliminate extra bits.*/
    red>>=8-bpp;
    green>>=8-bpp;
    blue>>=8-bpp;

    /*next left shift red and green back to their proper position as the would be in 6 bits rgb. blue doesn't need to be shifted.*/
    red<<=4;
    green<<=2;

    pixel=red|green|blue;

    fputc(pixel,fp); /*finally output the index to palette array*/
    bitcount+=8; /*add 8 to bitcount each time a 1 byte pixel is written*/

   x++;
  }

  /*This loop fixes things when the image is not a multiple of 32 bits in width.*/
  while(bitcount%32!=0)
  {
   fputc(0,fp);
   bitcount+=8;
  }

 }

 /*start of header fill code*/

 x=ftell(fp); /*get the size of the file now that it has been written.*/
 fseek(fp,2,SEEK_SET); /*seek back to offset 2*/
 fputint(x,fp,4);

 fseek(fp,0xA,SEEK_SET); /*seek back to offset A*/
 fputint(data,fp,4); /*Tell it where the pixels start!*/

 fseek(fp,0xE,SEEK_SET); /*seek back to location for dib header size*/
 fputint(12,fp,4); /*The size of this header (12 bytes)*/

 fseek(fp,0x12,SEEK_SET); /*Seek to width and height location*/
 fputint(width,fp,2); /*The bitmap width in pixels (unsigned 16-bit)*/
 fputint(height,fp,2); /*The bitmap height in pixels (unsigned 16-bit)*/

 fseek(fp,0x16,SEEK_SET);
 fputint(1,fp,2); /*The number of color planes, must be 1*/

 fseek(fp,0x18,SEEK_SET); /*Bits Per Pixel location*/
 fputint(8,fp,2); /*8 according to the Microsoft API even though only 6 bits are used in this function.*/

 /*end of header fill code*/

 fclose(fp);

 /*printf("Saved to file: %s\n",filename);*/

}



/*
I originally copied this function from my main BBM pixel loading function. It has been heavily modified so that it can load the pixels from monochrome BMP files when called from the function which follows it. It's sensitive and may not work reliably but I have had some success with it.
I also added support for 24 bit RGB images.
11-12-2019 update: added support for loading 32 bpp images.
*/
void BBM_Load_BMP_Pixels(uint32_t *p,uint32_t width,uint32_t height,FILE* fp,uint32_t bpp)
{
 uint32_t x,y,pixel,x2,c,bitcount,bits,r,g,b;

 if(bpp==1)
 {
  y=height;
  while(y>0)
  {
   y--;
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

   while(bitcount%32!=0)
   {
    if(bitcount%8==0)
    {
     c=fgetc(fp);
     if(feof(fp))
     {
      printf("Error: End of file reached.\n");
      printf("While skipping row padding.\n");
     }
    }

    bitcount++;
   }
  }
 } /*end of 1 bpp section*/

 else if(bpp==24)
 {
  printf("Type of image is 24 bpp or full color RGB\n");


  y=height;
  while(y>0)
  {
   y--;
   bitcount=0;
   x=0;
   while(x<width)
   {

    pixel=0;
    b=fgetc(fp);
    g=fgetc(fp);
    r=fgetc(fp);

    pixel|=r<<16;
    pixel|=g<<8;
    pixel|=b;


    p[x+y*width]=pixel;

    bitcount+=bpp;

    x++;
   }

   /*printf("bitcount==%d\n",bitcount);*/

   while(bitcount%32!=0)
   {
    c=fgetc(fp);
    if(feof(fp))
    {
     printf("Error: End of file reached.\n");
     printf("While skipping row padding.\n");
    }
    bitcount+=8;
   }

  }
 } /*end of 24 bit code*/

else if(bpp==32)
 {
  printf("Type of image is 32 bpp or full color RGB\n");

  y=height;
  while(y>0)
  {
   y--;
   bitcount=0;
   x=0;
   while(x<width)
   {
    pixel=0;
    b=fgetc(fp);
    g=fgetc(fp);
    r=fgetc(fp);
    fgetc(fp);   /*skip the alpha channel*/

    pixel|=r<<16;
    pixel|=g<<8;
    pixel|=b;

    p[x+y*width]=pixel;

    bitcount+=bpp;
    x++;
   }
  }
 } /*end of 32 bit code*/

 else
 {
  printf("Cannot load image of %d Bits Per Pixel\n",bpp);
 }



}


/*
This function has the limited ability to load certain types of BMP files. Currently it works with the 12 or 40 byte DIB header. Even then, it only works with monochrome AKA 1 bit per pixel images or 24 bit per pixel rgb images. These are the most useful types of files in my experience. It uses the above function for the pixel loading but only after extracting all the variables with image needed for displaying the data.
*/
void BBM_Load_BMP(uint32_t **p,uint32_t *width,uint32_t *height,const char *filename)
{
 int bpp,data,dib_size;
 FILE* fp;
 fp=fopen(filename,"rb");
 /*printf("This function loads a Microsoft BMP file into memory.\n");*/
 if(fp==NULL){printf("Failed to read file \"%s\": Doesn't exist.\n",filename); *p=NULL; return;}
 else{printf("File \"%s\" opened for reading.\n",filename);}

 fseek(fp,0xE,SEEK_SET); /*seek to location of header size*/
 dib_size=fgetint(fp,4);
 printf("dib_size=%d\n",dib_size);

 if(dib_size==12)
 {
  printf("dib header of %d bytes is supported!\n",dib_size);
  fseek(fp,0xA,SEEK_SET);
  data=fgetint(fp,4);
  printf("Pixel Data Starts at address=%d\n",data);
  fseek(fp,0x12,SEEK_SET); /*Seek to width and height location*/
  *width=fgetint(fp,2);
  *height=fgetint(fp,2);
/*
  printf("Width=%d\n",*width);
  printf("Height=%d\n",*height);
*/
  fseek(fp,0x18,SEEK_SET);
  bpp=fgetint(fp,2);
  printf("Bits Per Pixel=%d\n",bpp);
  fseek(fp,data,SEEK_SET);
 }
 else if(dib_size==40||dib_size==124)
  {
  printf("dib header of %d bytes is supported!\n",dib_size);
  fseek(fp,0xA,SEEK_SET);
  data=fgetint(fp,4);
  printf("Pixel Data Starts at address=%d\n",data);
  fseek(fp,0x12,SEEK_SET); /*Seek to width and height location*/
  *width=fgetint(fp,4);
  *height=fgetint(fp,4);
  printf("Width=%d\n",*width);
  printf("Height=%d\n",*height);
  fseek(fp,0x1C,SEEK_SET);
  bpp=fgetint(fp,2);
  printf("Bits Per Pixel=%d\n",bpp);
  fseek(fp,data,SEEK_SET);
 }
 else
 {
  printf("Because the DIB header size was not an understood size by my code\n");
  printf("It may be a valid file but I need to write code for it\n");
  return;
 }

 /*
  As a speed optimization, only allocate memory if the pointer is NULL.
  Otherwise assume it has enough memory already. Otherwise it allocates and frees every time which is slow for hundreds of frames in a row.
 */
 if(*p==NULL)
 {
  *p=BBM_malloc(*width,*height);
 }

 

 BBM_Load_BMP_Pixels(*p,*width,*height,fp,bpp);

 fclose(fp);
}




/*
Because the BMP format does support 4 bpp images. It's possible to do a grayscale format that stores 16 shades of gray. Each byte therefore has the data for two pixels. This makes the image take about half as much space as an 8bpp image without much difference in appearance.

By the power of grayscale!
*/
void BBM_SaveBMP_Gray16(uint32_t *p,int width,int height,const char* filename)
{
 int x,y,data,bpp=4;
 FILE* fp;
 fp=fopen(filename,"wb+");
 if(fp==NULL){printf("Failed to create file \"%s\".\n",filename); return;}
 else{/*printf("File \"%s\" opened.\n",filename);*/}

 fputint(0x4D42,fp,0x1A); /*26 bytes because 14 file header + 12 dib header*/

 /*Set up the pallete*/
 x=0;
 while(x<256)
 {
  fputc(x,fp);fputc(x,fp);fputc(x,fp);
  x+=0x11;
 }

 data=ftell(fp);

 y=height;
 while(y>0)
 {
  int bitcount=0,byte=0;
  y--;
  x=0;
  while(x<width)
  {
   int pixel=p[x+y*width];
   int nibble=Cast_Gray(pixel)>>4;
   byte<<=4;
   byte+=nibble;
   bitcount+=4;
   if(bitcount%8==0)
   {
    fputc(byte,fp);
   }
   x++;
  }

  /*This loop fixes things when the image is not a multiple of 32 bits in width.*/
  while(bitcount%32!=0)
  {
   byte<<=4;
   bitcount+=4;
   if(bitcount%8==0)
   {
    fputc(byte,fp);
   }
  }

 }

 /*start of header fill code*/

 x=ftell(fp); /*get the size of the file now that it has been written.*/
 fseek(fp,2,SEEK_SET); /*seek back to offset 2*/
 fputint(x,fp,4);

 fseek(fp,0xA,SEEK_SET); /*seek back to offset A*/
 fputint(data,fp,4); /*Tell it where the pixels start!*/

 fseek(fp,0xE,SEEK_SET); /*seek back to location for dib header size*/
 fputint(12,fp,4); /*The size of this header (12 bytes)*/

 fseek(fp,0x12,SEEK_SET); /*Seek to width and height location*/
 fputint(width,fp,2); /*The bitmap width in pixels (unsigned 16-bit)*/
 fputint(height,fp,2); /*The bitmap height in pixels (unsigned 16-bit)*/

 fseek(fp,0x16,SEEK_SET);
 fputint(1,fp,2); /*The number of color planes, must be 1*/

 fseek(fp,0x18,SEEK_SET); /*Bits Per Pixel location*/
 fputint(bpp,fp,2);

 /*end of header fill code*/


 fclose(fp);

 printf("Saved to file: %s\n",filename);
}



/*
Because the BMP format does support 8 bpp images. It's possible to do a grayscale format that mimics the PGM format with perfect accuracy! By the power of grayscale!
*/
void BBM_SaveBMP_Gray(uint32_t *p,int width,int height,const char* filename)
{
 int x,y,data,bpp=8;
 FILE* fp;
 fp=fopen(filename,"wb+");
 if(fp==NULL){printf("Failed to create file \"%s\".\n",filename); return;}
 else{/*printf("File \"%s\" opened.\n",filename);*/}

 fputint(0x4D42,fp,0x1A); /*26 bytes because 14 file header + 12 dib header*/

 /*Set up the pallete*/
 x=0;
 while(x<256)
 {
  fputc(x,fp);fputc(x,fp);fputc(x,fp);
  x++;
 }

 data=ftell(fp);
 y=height;
 while(y>0)
 {
  int bytecount=0;
  y--;
  x=0;
  while(x<width)
  {
   int pixel=p[x+y*width];
   fputc(Cast_Gray(pixel),fp);
   bytecount++;
   x++;
  }

  while(bytecount%4!=0)
  {
   fputc(0,fp);
   bytecount++;
  }
 }

 /*start of header fill code*/

 x=ftell(fp); /*get the size of the file now that it has been written.*/
 fseek(fp,2,SEEK_SET); /*seek back to offset 2*/
 fputint(x,fp,4);

 fseek(fp,0xA,SEEK_SET); /*seek back to offset A*/
 fputint(data,fp,4); /*Tell it where the pixels start!*/

 fseek(fp,0xE,SEEK_SET); /*seek back to location for dib header size*/
 fputint(12,fp,4); /*The size of this header (12 bytes)*/

 fseek(fp,0x12,SEEK_SET); /*Seek to width and height location*/
 fputint(width,fp,2); /*The bitmap width in pixels (unsigned 16-bit)*/
 fputint(height,fp,2); /*The bitmap height in pixels (unsigned 16-bit)*/

 fseek(fp,0x16,SEEK_SET);
 fputint(1,fp,2); /*The number of color planes, must be 1*/

 fseek(fp,0x18,SEEK_SET); /*Bits Per Pixel location*/
 fputint(bpp,fp,2);

 /*end of header fill code*/

 fclose(fp);

 printf("Saved to file: %s\n",filename);
}


/*
Displaying graphics normally requires huge APIs that interact with your operating system to open windows and draw to them or requires creating image files which can be viewed in already existing programs you have installed and know how to use.

The following is a function I created which specifically creates a Windows BMP file.
For a full understanding of the format I read extensively from this page:

https://en.wikipedia.org/wiki/BMP_file_format

And now I have everything I need to show what my images look like.
*/
void BBM_SaveBMP_RGB(uint32_t *pointer,uint32_t width,uint32_t height,const char* filename)
{
 uint32_t x,y,pixelpointer;
 FILE* fp;
 fp=fopen(filename,"wb+");
 if(fp==NULL){printf("Failed to create file \"%s\".\n",filename); return;}
 fputint(0x4D42,fp,0x1A); /*26 bytes because 14 file header + 12 dib header*/
 pixelpointer=ftell(fp);
 y=height;
 while(y>0)
 {
  uint32_t bytecount=0;
  y--;
  x=0;
  while(x<width)
  {
   fputint(pointer[x+y*width],fp,3);
   bytecount+=3;
   x++;
  }
  while(bytecount&3){fputc(0,fp);bytecount++;}
 }
 x=ftell(fp); /*get the size of the file now that it has been written.*/
 fseek(fp,2,SEEK_SET); /*seek back to offset 2*/
 fputint(x,fp,4); /*write file size*/
 fseek(fp,0xA,SEEK_SET); /*seek to location of pixel pointer*/
 fputint(pixelpointer,fp,4); /*Tell it where the pixels start!*/
 fputint(12,fp,4); /*The size of this header (12 bytes)*/
 fputint(width,fp,2); /*The bitmap width in pixels (unsigned 16-bit)*/
 fputint(height,fp,2); /*The bitmap height in pixels (unsigned 16-bit)*/
 fputint(1,fp,2); /*The number of color planes, must be 1*/
 fputint(24,fp,2); /*Bits Per Pixel*/
 fclose(fp);
}


/*
This calls the correct functions above based on a bits per pixel variable passed to the function.
24 is most common as it's full color with no restriction. 1 is for black and white images.
*/
void BBM_Save_BMP(uint32_t *p,int width,int height,const char* filename,int bpp)
{
      if(bpp==1){BBM_SaveBMP_BW(p,width,height,filename);}
 else if(bpp==4){BBM_SaveBMP_Gray16(p,width,height,filename);}
 else if(bpp==8){BBM_SaveBMP_Gray(p,width,height,filename);}

 else if(bpp==3){ BBM_Save_BMP_RGB_3(p,width,height,filename);}
 else if(bpp==6){ BBM_Save_BMP_RGB_6(p,width,height,filename);}
 else if(bpp==24){BBM_SaveBMP_RGB(p,width,height,filename);}

 else {printf("Currently only 1,4,8,3,6 or 24 bit BMP supported.\n");}
}

/*
A function to save a Microsoft Bitmap at 4 different bit depths using the above function.
*/
void BBM_BMP_Test(uint32_t *p,int width,int height)
{
 BBM_Save_BMP(p,width,height,"o/bbm.bmp",1);
 BBM_Save_BMP(p,width,height,"o/bbm4.bmp",4);
 BBM_Save_BMP(p,width,height,"o/bbm8.bmp",8);
 BBM_Save_BMP(p,width,height,"o/bbm24.bmp",24);
}


/*This function converts a BBM file to 1 bpp BMP.*/
void BBM_to_BMP(const char* filename,const char* filename1)
{
 uint32_t width=0,height=0; uint32_t *p;
 printf("This function converts a BBM file to 1 bpp BMP\n");
 BBM_LoadBBM(&p,&width,&height,filename);
 if(p==NULL){printf("Could not read a valid BBM file!\n");return;}
 BBM_SaveBMP_BW(p,width,height,filename1);
 free(p);
}

/*
convert my own BBM type file to any other by means of first converting it to a Microsoft bitmap with the function above. Then use imagemagick to convert it to any other type of file supported!

Of course this works on my PC because I have the magick tool installed and may not work on other people's who are not graphics programmers.
*/
void BBM_magick(const char* infile,const char* outfile)
{
 char command[256];
 char tmp[]="temp.bmp";
 BBM_to_BMP(infile,tmp);
 sprintf(command,"magick convert %s %s",tmp,outfile);
 printf("%s\n",command);
 system(command);
}
