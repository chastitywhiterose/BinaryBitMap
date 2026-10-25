/*
this is meant to have functions for creating xpm files. xpm files are a lot like xbm except that they take more space and can do more colors
*/


/*
this is the function for creating 1 bit per pixel or black and white images
though of course this is a plain text form that does not pack the pixels but is highly readable in a text editor.
*/
void BBM_Save_XPM_1(uint32_t *pointer,uint32_t width,uint32_t height,const char* filename)
{
 uint32_t x,y,pixel,r,g,b,gray,bit;
 FILE* fp;
 fp=fopen(filename,"wb+");
 if(fp==NULL){printf("Failed to create file \"%s\".\n",filename); return;}
 else{printf("File \"%s\" opened for writing.\n",filename);}
 fprintf(fp,"/* XPM */\n");
 fprintf(fp,"static char *bbm[] = {\n");
 fprintf(fp,"/* columns rows colors chars-per-pixel */\n");
 fprintf(fp,"\"%u %u 2 1\",\n",width,height);
 fprintf(fp,"\"0 c black\",\n");
 fprintf(fp,"\"1 c white\",\n");
 fprintf(fp,"/* pixels */\n");

 y=0;
 while(y<height)
 {
  fprintf(fp,"\"");
  x=0;
  while(x<width)
  {
   pixel=pointer[x+y*width];
   r=(pixel&0xFF0000)>>16;
   g=(pixel&0x00FF00)>>8;
   b=(pixel&0x0000FF);
   gray=(r+g+b)/3;
   bit=gray>>7;
   fputc(bit|0x30,fp);
   x++;
  }
  fprintf(fp,"\",\n");
  y++;
 }
 fprintf(fp,"};");
 fclose(fp);
}



/*this is the function for creating 3 bit per pixel or 8 color images*/
void BBM_Save_XPM_3(uint32_t *pointer,uint32_t width,uint32_t height,const char* filename)
{
 uint32_t x,y,pixel,r,g,b;
 FILE* fp;
 fp=fopen(filename,"wb+");
 if(fp==NULL){printf("Failed to create file \"%s\".\n",filename); return;}
 else{printf("File \"%s\" opened for writing.\n",filename);}
 fprintf(fp,"/* XPM */\n");
 fprintf(fp,"static char *bbm[] = {\n");
 fprintf(fp,"/* columns rows colors chars-per-pixel */\n");
 fprintf(fp,"\"%u %u 8 1\",\n",width,height);
 fprintf(fp,"\"0 c black\",\n");
 fprintf(fp,"\"1 c blue\",\n");
 fprintf(fp,"\"2 c green\",\n");
 fprintf(fp,"\"3 c cyan\",\n");
 fprintf(fp,"\"4 c red\",\n");
 fprintf(fp,"\"5 c magenta\",\n");
 fprintf(fp,"\"6 c yellow\",\n");
 fprintf(fp,"\"7 c white\",\n");
 fprintf(fp,"/* pixels */\n");

 y=0;
 while(y<height)
 {
  fprintf(fp,"\"");
  x=0;
  while(x<width)
  {
   pixel=pointer[x+y*width];
   r=(pixel&0xFF0000)>>16;
   g=(pixel&0x00FF00)>>8;
   b=(pixel&0x0000FF);
   r>>=7;
   g>>=7;
   b>>=7;
   r<<=2;
   g<<=1;
   pixel=r|g|b;
   fputc(pixel|0x30,fp);
   x++;
  }
  fprintf(fp,"\",\n");
  y++;
 }
 fprintf(fp,"};");
 fclose(fp);
}



/*

There are three ways I can get an XPM from a bmp file. First I create a BMP file then convert it with one of these tools.

magick convert ./o/bbm.bmp ./o/bbm.xpm
gm convert ./o/bbm.bmp ./o/bbm.xpm

And if ffmpeg supported xpm this would theoretically work.
ffmpeg -i ./o/bbm.bmp ./o/bbm.xpm

More info on formats supported by these tools
https://imagemagick.org/script/formats.php
http://www.graphicsmagick.org/formats.html
https://ffmpeg.org/general.html#Image-Formats
*/