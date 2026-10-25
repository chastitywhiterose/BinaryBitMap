/*
A header file all about making PAM files.

The PAM format is well documented and is part of the NetPBM series of formats. It can store monochrome,grayscale, and full color RGB images. It is just as great as any other format or perhaps better because it's easy to write the format.

http://netpbm.sourceforge.net/doc/pam.html

Converting from PAM to PNG
http://netpbm.sourceforge.net/doc/pnmtopng.html
http://netpbm.sourceforge.net/doc/pamtopng.html

Or the reverse. PNG to PAM
http://netpbm.sourceforge.net/doc/pngtopam.html

Or if you need to convert a jpeg into one of the formats:

http://netpbm.sourceforge.net/doc/jpegtopnm.html

jpegtopnm <RoseRomance.jpg >RoseRomance.pnm

It could be converted into any other image format.

To turn a PBM,PGM,or PPM file into a PAM:
pamtopam <o/bbm.pbm >o/bbm.pam
pamtopam <o/bgm.pgm >o/bgm.pam
pamtopam <o/bpm.ppm >o/bpm.pam

To turn any of the 4 formats into a PNG file the old way:
pnmtopng <o/bbm.pbm >o/bbm.png

Or the new way:
pamtopng <o/bbm.pbm >o/bbm.png

*/

/*
Saves to PAM. A newer format with less support but mentioned on the netpbm website. This version is the black and white only type. The PAM format can have much variety but I haven't yet put in the work to write each type. This one worked last I checked however! This type of image can be converted to PNG by other programs and uploaded to any website!
*/
void BBM_SavePAM_BW(uint32_t *p,uint32_t width,uint32_t height,const char* filename)
{
 uint32_t x,y,pixel,r,g,b,gray,bits,bpp=1;
 FILE* fp;
 fp=fopen(filename,"wb+");
 if(fp==NULL){printf("Failed to create file \"%s\".\n",filename); return;}
 fprintf(fp,"P7\n");
 fprintf(fp,"WIDTH %d\nHEIGHT %d\n",width,height);
 fprintf(fp,"DEPTH 1\n");
 fprintf(fp,"MAXVAL 1\n");
 fprintf(fp,"TUPLTYPE BLACKANDWHITE\n");
 fprintf(fp,"ENDHDR\n");
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
   bits=gray>>(8-bpp);
   fputc(bits,fp);
   x++;
  }
  y++;
 }
 fclose(fp);
 printf("Saved to file: %s\n",filename);
}




/*
The Grayscale version of PAM!
The function was written under the assumption that you would pass a bpp to it of 1,2,4, or 8. There is no error checking in it so if you do something outside of those, you'll get unexpected results. But you might get away with any number from 1 to 8.

The result is to shift the gray values right based on the bpp and could result in shades of gray ranging from 0 to (1<<bpp)-1. For example a bpp of 8 gives the traditional 0 to 255 range.
*/
void BBM_SavePAM_Gray(uint32_t *p,uint32_t width,uint32_t height,const char* filename,uint32_t bpp)
{
 uint32_t x,y,pixel,r,g,b,gray,bits;
 FILE* fp;
 fp=fopen(filename,"wb+");
 if(fp==NULL){printf("Failed to create file \"%s\".\n",filename); return;}
 fprintf(fp,"P7\n");
 fprintf(fp,"WIDTH %d\nHEIGHT %d\n",width,height);
 fprintf(fp,"DEPTH 1\n");
 fprintf(fp,"MAXVAL %d\n",(1<<bpp)-1);
 fprintf(fp,"TUPLTYPE GRAYSCALE\n");
 fprintf(fp,"ENDHDR\n");
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
   bits=gray>>(8-bpp);
   fputc(bits,fp);
   x++;
  }
  y++;
 }
 fclose(fp);
 printf("Saved to file: %s\n",filename);
}



/*
The full color RGB version of PAM!
The function was written under the assumption that you would pass a bpp to it of 3,6,12, or 24.

There is no error checking in it so if you do something outside of those, you'll get unexpected results. But you might get away with any multiple of 3 up to 24. The bpp will be divided by 3 and used to shift each Red,Green,and Blue part of each pixel based on it. This allows reducing the number of colors to hopefully make smaller png files if pnmtopng or another tool is used to convert it.

*/
void BBM_SavePAM_RGB(uint32_t *p,uint32_t width,uint32_t height,const char* filename,uint32_t bpp)
{
 uint32_t x,y,pixel,r,g,b;
 FILE* fp;
 fp=fopen(filename,"wb+");
 if(fp==NULL){printf("Failed to create file \"%s\".\n",filename); return;}
 bpp/=3;
 fprintf(fp,"P7\n");
 fprintf(fp,"WIDTH %d\nHEIGHT %d\n",width,height);
 fprintf(fp,"DEPTH 3\n");
 fprintf(fp,"MAXVAL %d\n",(1<<bpp)-1);
 fprintf(fp,"TUPLTYPE RGB\n");
 fprintf(fp,"ENDHDR\n");
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
   r>>=(8-bpp);
   g>>=(8-bpp);
   b>>=(8-bpp);
   fputc(r,fp);
   fputc(g,fp);
   fputc(b,fp);
   x++;
  }
  y++;
 }
 fclose(fp);
 printf("Saved to file: %s\n",filename);
}

/*
A function which calls the relevant PAM function above based on the Bits Per Pixel.
*/
void BBM_SavePAM(uint32_t *p,uint32_t width,uint32_t height,const char* filename,uint32_t bpp)
{
 if(bpp==1) {BBM_SavePAM_BW(p,width,height,filename); return;}
 if(bpp==2||bpp==4||bpp==8) {BBM_SavePAM_Gray(p,width,height,filename,bpp); return;}
 if(bpp==3||bpp==6||bpp==12||bpp==24) {BBM_SavePAM_RGB(p,width,height,filename,bpp); return;}

 printf("Bits Per Pixel of %d is invalid for this function!\n",bpp);
}

/*
A function with one purpose. To test my other functions by writing PAM files and then converting them to PNG files for actual viewing. This allows me to see what an image would look like at different bit depths that I've defined without relying on special image viewers. The whole point of the PAM format is because it converts to PNG so well.
*/
void BBM_PAM_Test(uint32_t *p,uint32_t width,uint32_t height)
{
 BBM_SavePAM(p,width,height,"o/bbm01.pam",1);
 BBM_SavePAM(p,width,height,"o/bbm02.pam",2);
 BBM_SavePAM(p,width,height,"o/bbm04.pam",4);
 BBM_SavePAM(p,width,height,"o/bbm08.pam",8);

 BBM_SavePAM(p,width,height,"o/bbm03.pam",3);
 BBM_SavePAM(p,width,height,"o/bbm06.pam",6);
 BBM_SavePAM(p,width,height,"o/bbm12.pam",12);
 BBM_SavePAM(p,width,height,"o/bbm24.pam",24);
/*
 pnmtopng("o/bbm1.pam","o/bbm1.png");
 pnmtopng("o/bbm2.pam","o/bbm2.png");
 pnmtopng("o/bbm4.pam","o/bbm4.png");
 pnmtopng("o/bbm8.pam","o/bbm8.png");

 pnmtopng("o/bbm3.pam","o/bbm3.png");
 pnmtopng("o/bbm6.pam","o/bbm6.png");
 pnmtopng("o/bbm12.pam","o/bbm12.png");
 pnmtopng("o/bbm24.pam","o/bbm24.png");
*/
 if(0){system("rm o/*.pam");}

}
 


