/*
A function which writes the pixels according to my own algorithm I decided was the best. Backwards compatible with BBM.
It accepts all the same types of arguments as the BGM function which follows except in this case the file is passed as a file pointer and is assumed to already be opened by another function so that this code can be used and move all of the important processing of pixels into it instead of it being copy/pasted into each other function I write. That's just a maintenancy nightmare as I quickly learned!
*/
void BBM_SaveBGM_Pixels(uint32_t *p,uint32_t width,uint32_t height,FILE* fp,int bpp)
{
 uint32_t x,y,bitcount=0,bits,c=0,pixel,r,g,b,gray;

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

 printf("Bits Per Pixel of %d is invalid for Binary Gray Map!\n",bpp);
}

/*
Saves to BGM: Binary Gray Map
The second format that I invented! The details of this format are as follows.

2 bytes for the width
2 bytes for the height
1 byte for the bits per pixel setting. Must be 1,2,4, or 8. If 1, then the image is identical to a BBM,which is black and white only. Otherwise it's grayscale with quality higher the more the bpp.
*/

void BBM_SaveBGM(uint32_t *p,int width,int height,const char *filename,int bpp)
{
 FILE* fp;
 fp=fopen(filename,"wb+");
 if(fp==NULL){printf("Failed to create file \"%s\".\n",filename); return;}
 else{printf("File \"%s\" opened for writing.\n",filename);}
 
 fputint(width,fp,2);
 fputint(height,fp,2);
 fputint(bpp,fp,1);

 BBM_SaveBGM_Pixels(p,width,height,fp,bpp);

 fclose(fp);
}



/*
A function to load my Binary Gray Map files.
*/
void BBM_LoadBGM(uint32_t **p,uint32_t *width,uint32_t *height,const char *filename)
{
 uint32_t x,y,bits,bitcount=0,c,bpp=1;
 FILE* fp;
 fp=fopen(filename,"rb");
 printf("This function loads a BGM file into memory.\n");
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

 y=0;
 while( y < (*height) )
 {
  x=0;
  while( x < (*width) )
  {
   uint32_t x2,pixel;
   if(bitcount%8==0)
   {
    c=fgetc(fp);
    if(feof(fp))
    {
     printf("Error: End of file reached.\n");
     free(*p); *p=NULL; return;
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

   (*p)[x+y*(*width)]=pixel;
   x++;
  }
  y++;
 }

 fclose(fp);
}
