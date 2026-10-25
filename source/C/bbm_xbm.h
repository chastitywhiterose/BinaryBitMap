/*Header for XBM and XPM formats from Xlib.*/

/*
Saves to XBM. Another great format.

Also can be converted like this
magick convert checker.pbm checker.xbm
*/

void BBM_SaveXBM(uint32_t *p,uint32_t width,uint32_t height,const char* filename)
{
 uint32_t x,y,textrow,pixel,bit,r,g,b,gray;
 FILE* fp;
 fp=fopen(filename,"wb+");
 if(fp==NULL){printf("Failed to create file \"%s\".\n",filename); return;}
 else{printf("File \"%s\" opened.\n",filename);}
 fprintf(fp,"#define bbm_width %d\n",width);
 fprintf(fp,"#define bbm_height %d\n",height);
 fprintf(fp,"static char bbm_bits[] = {\n");
 y=0;
 while(y<height)
 {
  int bitcount=0;
  unsigned char c=0;
  textrow=0;
  x=0;
  while(x<width)
  {
   pixel=p[x+y*width];
   r=(pixel&0xFF0000)>>16;
   g=(pixel&0x00FF00)>>8;
   b=(pixel&0x0000FF);
   gray=(r+g+b)/3;
   bit=gray>>7;
   bit^=1;
   c>>=1;
   c|=bit<<7;
   bitcount++;
   x++;
   if(bitcount==8)
   {
    if(textrow==0){fprintf(fp," ");}
    fprintf(fp,"0x%02X,",c);
    bitcount=0;
    c=0;
    textrow++;
   }
   if(textrow==16){fprintf(fp,"\n");textrow=0;}
  }

    /*This loop fixes things when the image is not a multiple of 8 in width.*/
    while(bitcount!=0)
    {
     c>>=1;
     bitcount++;
     if(bitcount==8)
     {
      fprintf(fp,"0x%02X,",c);
      bitcount=0;
     }
    }

  y++;
 }
 fprintf(fp,"};\n");
 fclose(fp);

 printf("Saved to file: %s\n",filename);

}



/*
magick convert ./o/bbm.bmp ./o/bbm.xbm
gm convert ./o/bbm.bmp ./o/bbm.xbm
ffmpeg -i ./o/bbm.bmp ./o/bbm.xbm


More info on formats supported by imagemagick and graphicsmagick
https://imagemagick.org/script/formats.php
http://www.graphicsmagick.org/formats.html
*/