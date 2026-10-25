/*
A header for the third and final format I am creating. It can do everything the BBM and BGM can do. The code isn't complete yet however.

However the format I plan to use is something I have been thinking of for some time. Here is the 16 byte header.

4 bytes for the width
4 bytes for the height

4 bytes for the bits per pixel
Accepted values for this will be 1,2,4,8,24.

4 bytes for the pointer to the data.

*/

/*
A function which writes the pixels according to my own algorithm I decided was the best. Backwards compatible with BBM and BGM pixel methods.

It accepts all the same types of arguments as the BGM and BPM functions except in this case the file is passed as a file pointer and is assumed to already be opened by another function so that this code can be used and move all of the important processing of pixels into it instead of it being copy/pasted into each other function I write. That's just a maintenance nightmare as I quickly learned!
*/
void BBM_SaveBPM_Pixels(uint32_t *p,uint32_t width,uint32_t height,FILE* fp,int bpp)
{
 uint32_t x,y,pixel,r,g,b,c=0,bitcount=0,bits,gray,bpt;
 /*If 24 bits then do this RGB routine then return.*/


 if(bpp==24)
 {
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
  return;
 }

 if(bpp==3||bpp==6||bpp==12||bpp==24)
 {
  bpt=bpp/3;
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
    r>>=(8-bpt);
    g>>=(8-bpt);
    b>>=(8-bpt);
    fputc(r,fp);
    fputc(g,fp);
    fputc(b,fp);
    x++;
   }
   y++;
  }

  return;
 }

 /*If bpp is 1,2,4, or 8, run the grayscale method.*/
 if(bpp==1||bpp==2||bpp==4||bpp==8)
 { 
  y=0;
  while(y<height)
  {
   x=0;
   while(x<width)
   {
    pixel=p[x+y*width];
    /*formula to make a shade of gray based on the red,green,and blue components.*/
    r=(pixel&0xFF0000)>>16;
    g=(pixel&0x00FF00)>>8;
    b=(pixel&0x0000FF);
    gray=(r+g+b)/3;
    bits=gray>>(8-bpp);
    c<<=bpp;
    c|=bits;
    bitcount+=bpp;
    x++;
    if(bitcount%8==0)
    {
     fputc(c,fp);
    }
   }
   y++;
  }

  /*
   If the number of bits written is not a multiple of 8, the rest are padded by bit shifting.
   This makes a full byte which is padded to the end to avoid end of file errors if the format is read.
   Sometimes there could be only a few bits which need to be read from that byte.
  */
  while(bitcount%8!=0)
  {
   c<<=bpp;
   bitcount+=bpp;
   if(bitcount%8==0)
   {
    fputc(c,fp);
   }
  }
  return;
 }

 printf("Bits Per Pixel of %d is invalid for Binary Pixel Map!\n",bpp);

 fprintf(fp,"Error: Bits Per Pixel of %d can't be done with this function.",bpp);
}


/*The following two lines are debugging code I used in some functions while testing.


  printf("Bits Per Pixel=%d\n",bpp);
  printf("Bits Per Third=%d\n",bpt);
  printf("X=%d Y=%d ",x,y);
  printf("R=%d G=%d B=%d\n",r,g,b);

*/




/*
Saves to BPM: Binary Pixel Map
The third format that I invented! The details of this format are as follows.

2 bytes for the width
2 bytes for the height

1 byte for the bits per pixel setting.

If 1, then the image is identical to a BBM,which is black and white only.
If 2,4, or 8, it's grayscale with quality higher the more the bpp.
However if it's 24, then it becomes a full color RGB image capable of accurately representing any color computers can do!

But because it's so complicated, it has to rely on the big function I wrote above to write the pixels correctly in those different cases!
*/

void BBM_SaveBPM(uint32_t *p,uint32_t width,uint32_t height,const char *filename,int bpp)
{
 uint32_t data=4;
 FILE* fp;
 fp=fopen(filename,"wb+");
 if(fp==NULL){printf("Failed to create file \"%s\".\n",filename); return;}
 else{printf("File \"%s\" opened for writing.\n",filename);}
 fputint(width,fp,4);
 fputint(height,fp,4);
 fputint(bpp,fp,4);
 fputint(data,fp,4);
 fprintf(fp,"Binary Pixel Map");
 data=ftell(fp);
 BBM_SaveBPM_Pixels(p,width,height,fp,bpp);
 fseek(fp,0xC,SEEK_SET);
 fputint(data,fp,4);
 fclose(fp);
}

/*
 Function which loads the pixels according to the bit depth.
 This is to be called only after the file has been opened and the file pointer is at the location where the pixels begin.
 The memory for enough pixels must also be loaded. This is always the case if called by the function which is below this one.
*/
void BBM_LoadBPM_Pixels(uint32_t *p,uint32_t width,uint32_t height,FILE* fp,int bpp)
{
 uint32_t x,y,pixel,r,g,b,c,bitcount=0,bits;

 /*If 24 bits then do this RGB routine then return.*/
 if(bpp==24) 
 {
  y=0;
  while(y<height)
  {
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

    x++;
   }
   y++;
  }
  return;
 }

 /*If bpp is 1,2,4, or 8, run the grayscale method.*/
 if(bpp==1||bpp==2||bpp==4||bpp==8)
 { 
  y=0;
  while(y<height)
  {
   x=0;
   while(x<width)
   {
    uint32_t x2,pixel;
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
    /*printf("(%d,%d) bits=%d\n",x,y,bits);*/

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

  return;
 }


 printf("Bits Per Pixel of %d is invalid for Binary Pixel Map!\n",bpp);
}

/*Load a BPM file relying heavily on the function above for the pixel loading.*/
void BBM_LoadBPM(uint32_t **p,uint32_t *width,uint32_t *height,const char *filename)
{
 uint32_t bpp;
 FILE* fp;
 fp=fopen(filename,"rb");
 printf("This function loads a BPM file into memory.\n");
 if(fp==NULL){printf("Failed to read file \"%s\": Doesn't exist.\n",filename); *p=NULL; return;}
 else{printf("File \"%s\" opened for reading.\n",filename);}

 *width=fgetint(fp,2);
 *height=fgetint(fp,2);
 bpp=fgetint(fp,1);
 printf("width=%d\n",*width);
 printf("height=%d\n",*height);
 printf("bpp=%d\n",bpp);

 if(*p!=NULL)
 {
  printf("Pointer is not NULL. Will free memory first\n"); free(*p); *p=NULL;
 }

 *p=BBM_malloc(*width,*height);

 BBM_LoadBPM_Pixels(*p,*width,*height,fp,bpp);

 fclose(fp);
}
