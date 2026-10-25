/*Header for WBMP format*/

/*
A function I wrote to produce the multibyte integer format of the WBMP format. It stores the 7 bit groups into an array and then prints them in reverse order. The first element of the array has the most significant bit set to 0 but all the others will have it set to 1.

See section 6.3.1 of the documentation here that I read to figure it out.

http://www.wapforum.org/what/technical/SPEC-WAESpec-19990524.pdf

Wikipedia gave me the direct link to the PDF file. It told me precisely how the format worked. I wrote this code based on the example.
*/
void fputintvar(unsigned long int i,FILE *fp)
{
 int x=0,c[sizeof(i)];
 while(i>0)
 {
  c[x]=i&0x7F;
  if(x>0){c[x]|=0x80;}
  i>>=7;
  x++;
 }

 while(x>0)
 {
  x--;
  fputc(c[x],fp);
 }
}

/*
 The function that saves the pixels to a WBMP file.
 0 is black and 1 is White.
 Each byte contains 8 pixels. One per bit.
*/
void BBM_SaveWBMP_Pixels(uint32_t *p,uint32_t width,uint32_t height,FILE* fp)
{
 uint32_t x,y,pixel,r,g,b,gray,bitcount,bits,bpp=1;

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


/*The code for the Wireless Bitmap format is complete. It was extremely easy to make as all I needed to do was write the fputintvar function above and then copy paste the code I used in the PBM function for the pixel data.
*/
void BBM_SaveWBMP(uint32_t *p,uint32_t width,uint32_t height,const char* filename)
{
 /*uint32_t x,y,pixel,bit,bits,r,g,b,gray,bpp=1;*/

 FILE* fp;
 fp=fopen(filename,"wb+");
 if(fp==NULL){printf("Failed to create file \"%s\".\n",filename); return;}
 /*printf("Making WBMP format\n");*/
 fputint(0,fp,2); /*Make the two zero bytes at the beginning.*/
 fputintvar(width,fp);
 fputintvar(height,fp);
 BBM_SaveWBMP_Pixels(p,width,height,fp);
 fclose(fp);
}

/*Reads a variable length integer to get data from the WBMP file.*/
unsigned long int fgetintvar(FILE *fp)
{
 unsigned long int i=0,c,flag=1;
 while(flag!=0)
 {
  c=fgetc(fp);
  i<<=7;
  i+=c&0x7F;
  flag=c&0x80;
 }
 return i;
}

void BBM_Load_WBMP_Pixels(uint32_t *p,uint32_t width,uint32_t height,FILE* fp,uint32_t bpp)
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


void BBM_LoadWBMP(uint32_t **p,uint32_t *width,uint32_t *height,const char *filename)
{

 FILE* fp;
 fp=fopen(filename,"rb");
 printf("This function loads a WBMP file into memory.\n");
 if(fp==NULL){printf("Failed to read file \"%s\": Doesn't exist.\n",filename); *p=NULL; return;}
 else{printf("File \"%s\" opened for reading.\n",filename);}

 fseek(fp,0x2,SEEK_SET);

 *width=fgetintvar(fp);
 *height=fgetintvar(fp);

 printf("width=%d\n",*width);
 printf("height=%d\n",*height);

 if(*p!=NULL)
 {
  printf("Pointer is not NULL. Will free memory first\n"); free(*p); *p=NULL;
 }

 *p=BBM_malloc(*width,*height);

 BBM_Load_WBMP_Pixels(*p,*width,*height,fp,1);

 fclose(fp);
}


