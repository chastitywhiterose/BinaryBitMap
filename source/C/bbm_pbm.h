/*
A header file for the NetPBM formats which includes PGM and PPM as well as the original PBM format.
*/


/*
 The function that saves the pixels to a PBM file.
 0 is black and 1 is White.
 Each byte contains 8 pixels. One per bit.
*/
void BBM_SavePBM_Pixels(uint32_t *p,uint32_t width,uint32_t height,FILE* fp)
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
   gray>>=8-bpp; gray^=1;
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
Saves to PBM. My favorite already existing format because of it's simplicity. Next to my own BBM format this is the most efficient uncompressed storage of black and white pixels I have seen, unless there is another format I don't know about.
*/
void BBM_SavePBM(uint32_t *p,uint32_t width,uint32_t height,const char* filename)
{
 FILE* fp;
 fp=fopen(filename,"wb+");
 if(fp==NULL){printf("Failed to create file \"%s\".\n",filename); return;}
 else{/*printf("File \"%s\" opened.\n",filename);*/}
 fprintf(fp,"P4\n"); fprintf(fp,"%d %d\n",width,height);

 BBM_SavePBM_Pixels(p,width,height,fp);

 fclose(fp);
 /*printf("Saved to file: %s\n",filename);*/
}


/*
A function meant to load the PBM format. Doable but complex still. Amazingly I got it to work! This means I can load any PBM file which basically means any image on the internet can be loaded but only if it contains black and white pixels. I used imagemagick to convert the original jpeg image into a pbm to test this function.
*/
void BBM_LoadPBM(uint32_t **p,uint32_t *width,uint32_t *height,const char* filename)
{
 char s[0x10];
 unsigned char c;
 uint32_t x,y,bit;
 FILE* fp;
 printf("This function loads a PBM file into memory.\n");
 fp=fopen(filename,"rb");
 if(fp==NULL){printf("Failed to read file \"%s\".\n",filename); *p=NULL; return;}
 else{printf("File \"%s\" opened.\n",filename);}

 fscanf(fp,"%s",s);
 if(!strcmp(s,"P4")){printf("Correct \"%s\" header!\n",s);}
 else{printf("\"%s\" is not the correct header!\n",s);}
 fscanf(fp,"%u",width);
 fscanf(fp,"%u",height);
 printf("width=%d\n",*width);
 printf("height=%d\n",*height);
 *p=BBM_malloc(*width,*height);

 fgetc(fp);

 y=0;
 while(y< (*height) )
 {
  int bitcount=0;
  x=0;
  while(x<(*width))
  {
   int pixel;
   if(bitcount==0)
   {
    c=fgetc(fp);
    if(feof(fp)){printf("Error: End of file reached.\n");}
   }
   bit=c>>7;
   if(bit==0){pixel=u32bw[1];}else{pixel=u32bw[0];}
   (*p)[x+y*(*width)]=pixel;
   c<<=1;
   c|=bit;
   bitcount++;
   x++;
   if(bitcount==8)
   {
    bitcount=0;
    c=0;
   }
  }
  y++;
 }
 fclose(fp);
}

/*Saves to PGM. Capable of grayscale images.*/
void BBM_SavePGM(uint32_t *p,int width,int height,const char* filename)
{
 int x,y,r=0,g=0,b=0,byte=0;
 FILE* fp;
 fp=fopen(filename,"wb+");
 if(fp==NULL){printf("Failed to create file \"%s\".\n",filename); return;}
 else{/*printf("File \"%s\" opened.\n",filename);*/}
 fprintf(fp,"P5\n"); fprintf(fp,"%d %d\n",width,height); fprintf(fp,"255\n");

 y=0;
 while(y<height)
 {
  x=0;
  while(x<width)
  {
   int pixel=p[x+y*width];
   r=(pixel&0xFF0000)>>16;
   g=(pixel&0x00FF00)>>8;
   b=(pixel&0x0000FF);
   byte=(r+g+b)/3;
   /*printf("r,g,b,byte=%d,%d,%d,%d\n",r,g,b,byte);*/
   fputc(byte,fp);
   x++;
  }
  y++;
 }

 fclose(fp);
 /*printf("Saved to file: %s\n",filename);*/
}


/* Loads a Portable Gray Map file. */
void BBM_LoadPGM(uint32_t **p,uint32_t *width,uint32_t *height,const char* filename)
{
 char s[0x10];
 int c;
 uint32_t x,y;
 FILE* fp;
 printf("This function loads a PGM file into memory.\n");
 fp=fopen(filename,"rb");
 if(fp==NULL){printf("Failed to read file \"%s\".\n",filename); *p=NULL; return;}
 else{printf("File \"%s\" opened.\n",filename);}

 fscanf(fp,"%s",s);
 if(!strcmp(s,"P5")){printf("Correct \"%s\" header!\n",s);}
 else{printf("\"%s\" is not the correct header!\n",s);}
 fscanf(fp,"%u",width);
 fscanf(fp,"%u",height);
 printf("width=%d\n",*width);
 printf("height=%d\n",*height);
 *p=BBM_malloc(*width,*height);

 fscanf(fp,"%u",&x);
 printf("Maxval=%d\n",x);

 fgetc(fp);

 y=0;
 while(y< (*height) )
 {
  x=0;
  while(x<(*width))
  {
   int pixel=0;
   c=fgetc(fp);
   if(feof(fp)){printf("Error: End of file reached.\n");}

   pixel|=c;
   pixel|=c<<8;
   pixel|=c<<16;
   
   (*p)[x+y*(*width)]=pixel;

   x++;
  }
  y++;
 }
 fclose(fp);
}


/*I wrote this function for saving a surface to a ppm file. The ppm format is easy enough to write.*/
void BBM_SavePPM(uint32_t *p,uint32_t width,uint32_t height,const char* filename)
{
 uint32_t x,y,r=0,g=0,b=0;
 FILE* fp;
 fp=fopen(filename,"wb");
 if(fp==NULL){printf("Failed to create file \"%s\".\n",filename); return;}
 else{/*printf("File \"%s\" opened.\n",filename);*/}
 fprintf(fp,"P6\n");
 fprintf(fp,"%d %d\n",width,height);
 fprintf(fp,"255\n");

 /*printf("ftell==%ld\n",ftell(fp));*/

 y=0;
 while(y<height)
 {
  x=0;
  while(x<width)
  {
   int pixel=p[x+y*width];
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
 /*printf("Saved to file: %s\n",filename);*/
}


/* Loads a Portable Pixel Map file. */
void BBM_LoadPPM(uint32_t **p,uint32_t *width,uint32_t *height,const char* filename)
{
 char s[0x10];
 uint32_t x,y;
 FILE* fp;
 /*printf("This function loads a PPM file into memory.\n");*/
 fp=fopen(filename,"rb");
 if(fp==NULL){printf("Failed to read file \"%s\".\n",filename); *p=NULL; return;}
 else{printf("File \"%s\" opened.\n",filename);}

 fscanf(fp,"%s",s);
 if(!strcmp(s,"P6")){/*printf("Correct \"%s\" header!\n",s);*/}
 else{printf("\"%s\" is not the correct header!\n",s);}
 fscanf(fp,"%u",width);
 fscanf(fp,"%u",height);
 /*printf("width=%d\n",*width);
   printf("height=%d\n",*height);*/
 *p=BBM_malloc(*width,*height);

 fscanf(fp,"%u",&x);
 /*printf("Maxval=%d\n",x);*/

 fgetc(fp);
 /*printf("ftell==%ld\n",ftell(fp));*/

 y=0;
 while(y< (*height) )
 {
  x=0;
  while(x<(*width))
  {
   int pixel=0,r=0,g=0,b=0;
   r=fgetc(fp);
   g=fgetc(fp);
   b=fgetc(fp);
   if(feof(fp)){printf("Error: End of file reached.\n");}

   pixel|=r<<16;
   pixel|=g<<8;
   pixel|=b;
   
   (*p)[x+y*(*width)]=pixel;

   x++;
  }
  y++;
 }
 fclose(fp);
}


/*Saves to the text only plain PBM. Takes more space but easier to write!*/
void BBM_SavePBM_P1(uint32_t *p,uint32_t width,uint32_t height,const char* filename)
{
 uint32_t x,y,bit,pixel;
 FILE* fp;
 fp=fopen(filename,"wb+");
 if(fp==NULL){printf("Failed to create file \"%s\".\n",filename); return;}
 else{printf("File \"%s\" opened.\n",filename);}
 fprintf(fp,"P1\n"); fprintf(fp,"%d %d\n",width,height);
 y=0;
 while(y<height)
 {
  x=0;
  while(x<width)
  {
   pixel=p[x+y*width];
   if(pixel==u32bw[0]){bit=1;}else{bit=0;}
   fprintf(fp,"%d",bit);
   x++;
  }
  fprintf(fp,"\n");
  y++;
 }
 fclose(fp);

 printf("Saved to file: %s\n",filename);

}



