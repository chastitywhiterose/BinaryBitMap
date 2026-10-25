/*
A file meant for writing PBM,PGM,and PPM files.

Although I have another source file named bbm_pbm.h which has functions for reading and writing these formats, I wanted to try writing a new function which can automatically create the correct image of those three formats based on a bits per pixel value passed as an argument, similar to what I did in pamio.h .

Regardless of which format is output, it can be converted to PNG files with the netpbm tool
*/

/*
Uses the ancient pnmtopng program if it is installed as a hack to allow for converting PAM files to PNG format.
It also works with the older three PBM, PGM, and PPM formats.
*/
void pnmtopng(const char* infile,const char* outfile)
{
 char command[256];
 sprintf(command,"pnmtopng <%s >%s",infile,outfile);
 printf("%s\n",command);
 system(command);
}


void BBM_SavePNM(uint32_t *p,uint32_t width,uint32_t height,const char* filename,uint32_t bpp)
{
 uint32_t x,y,pixel,bit,bits,r,g,b,gray;


 FILE* fp;
 fp=fopen(filename,"wb+");
 if(fp==NULL){printf("Failed to create file \"%s\".\n",filename); return;}

 if(bpp==1)
 {
  printf("BPP=%d ",bpp);
  printf("Making PBM format\n");

  fprintf(fp,"P4\n"); fprintf(fp,"%d %d\n",width,height);

  y=0;
  while(y<height)
  {
   int bitcount=0;
   bits=0;
   x=0;
   while(x<width)
   {
    pixel=p[x+y*width];

    r=(pixel&0xFF0000)>>16;
    g=(pixel&0x00FF00)>>8;
    b=(pixel&0x0000FF);
    gray=(r+g+b)/3;
    bit=gray>>(8-bpp);
    bit^=1;

    bits<<=1;
    bits|=bit;
    bitcount++;
    x++;
    if(bitcount==8)
    {
     fputc(bits,fp);
     bitcount=0;
    }
  }
  /*This loop fixes things when the image is not a multiple of 8 in width.*/
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

 else if(bpp==2||bpp==4||bpp==8)
 {
  printf("BPP=%d ",bpp);
  printf("Making PGM format\n");

  fprintf(fp,"P5\n"); fprintf(fp,"%d %d\n",width,height); fprintf(fp,"%d\n",(1<<bpp)-1);

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

 }

 else if(bpp==3||bpp==6||bpp==12||bpp==24)
 {

  printf("BPP=%d ",bpp);
  printf("Making PPM format\n");

  bpp/=3;
  fprintf(fp,"P6\n"); fprintf(fp,"%d %d\n",width,height); fprintf(fp,"%d\n",(1<<bpp)-1);

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

 }

 else{printf("bpp of %d is invalid\n",bpp);}

 fclose(fp);
 printf("Saved to file: %s\n",filename);
}


/*
A function with one purpose. To test my other functions by writing PNM files and then converting them to PNG files for actual viewing. This allows me to see what an image would look like at different bit depths that I've defined without relying on special image viewers.
*/
void BBM_PNM_Test(uint32_t *p,uint32_t width,uint32_t height)
{
 BBM_SavePNM(p,width,height,"o/bbm1.pnm",1);
 BBM_SavePNM(p,width,height,"o/bbm2.pnm",2);
 BBM_SavePNM(p,width,height,"o/bbm4.pnm",4);
 BBM_SavePNM(p,width,height,"o/bbm8.pnm",8);

 BBM_SavePNM(p,width,height,"o/bbm3.pnm",3);
 BBM_SavePNM(p,width,height,"o/bbm6.pnm",6);
 BBM_SavePNM(p,width,height,"o/bbm12.pnm",12);
 BBM_SavePNM(p,width,height,"o/bbm24.pnm",24);

 pnmtopng("o/bbm1.pnm","o/bbm1.png");
 pnmtopng("o/bbm2.pnm","o/bbm2.png");
 pnmtopng("o/bbm4.pnm","o/bbm4.png");
 pnmtopng("o/bbm8.pnm","o/bbm8.png");

 pnmtopng("o/bbm3.pnm","o/bbm3.png");
 pnmtopng("o/bbm6.pnm","o/bbm6.png");
 pnmtopng("o/bbm12.pnm","o/bbm12.png");
 pnmtopng("o/bbm24.pnm","o/bbm24.png");

 if(1){system("rm o/*.pnm");}

}
