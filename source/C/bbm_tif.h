/*
Header for TIFF files

I read the specification for TIFF which I got here:
https://www.adobe.io/open/standards/TIFF.html

The values used to contruct the IFD make no sense unless you understand exactly what is involved and what all the values mean.

My fputint functin from bbmio.h was actually made for this purpose. To be able to specifically write values in little endian format for TIFF files using the precise number of bytes. It works well for other purposes as well of course such as cheating in video games.
*/

/*
The first step in adding TIFF support was to use Magick to convert a PBM file to a TIFF. I looked at a sample file to get a feel for the Bilevel TIFF format.

magick o/bbm.pbm o/bbm.tiff
*/

/*
Constant array of the first 8 bytes of every TIFF file.
This is the header. The last 4 zero bytes will be replaced by the address of the IFD. The other functions will handle that by seeking back to it and using my fputint function.
*/
unsigned char TIFF_Header[]={0x49,0x49,0x2A,0x00,0x00,0x00,0x00,0x00};



/*
Function to make black and white only TIFF files! My favorite kind!
*/

void BBM_SaveTIF_BW(uint32_t *p,uint32_t width,uint32_t height,const char *filename)
{
 uint32_t x,y,bit,pixel,c,IFD_Address,Pixels_Address;
 FILE* fp;
 fp=fopen(filename,"wb+");
 if(fp==NULL){printf("Failed to create file \"%s\".\n",filename); return;}
 else{printf("File \"%s\" opened for writing.\n",filename);}
 
 fwrite(TIFF_Header,sizeof(char),sizeof(TIFF_Header),fp);

 Pixels_Address=ftell(fp);

 y=0;
 while(y<height)
 {
  int bitcount=0;
  c=0;
  x=0;
  while(x<width)
  {
   pixel=p[x+y*width];
   bit=Cast_BW(pixel);
   c<<=1;
   c|=bit;
   bitcount++;
   x++;
   if(bitcount==8)
   {
    fputc(c,fp);
    bitcount=0;
    c=0;
   }
  }
  /*This loop fixes things when the image is not a multiple of 8 in width.*/
  while(bitcount!=0)
  {
   c<<=1;
   bitcount++;
   if(bitcount==8)
   {
    fputc(c,fp);
    bitcount=0;
   }
  }
  y++;
 }

 IFD_Address=ftell(fp); /*IFD begins after pixel data. Get the current location.*/

 fputint(0,fp,2); /*write two zero bytes. Later on we will replace this with the number of entries in this IFD*/

 /*Each IFD entry is exactly 12 bytes.
  2 for the magic number identifier.
  2 for the data type
  4 for the count(most tags are a count of 1 but there are exceptions).
  4 for the value(or pointer to data elsewhere in the case of an array)
 */
 fputint(  256,fp,2); /*Tag for image width*/
 fputint(    3,fp,2); /*3 for SHORT data type. See page 15 of TIFF 6.0 documentation.*/
 fputint(    1,fp,4); /*count is 1*/
 fputint(width,fp,4); /*Actual value of width*/

 fputint(  257,fp,2); /*Tag for image height*/
 fputint(    3,fp,2); /*3 for SHORT data type. See page 15 of TIFF 6.0 documentation.*/
 fputint(    1,fp,4); /*count is 1*/
 fputint(height,fp,4); /*Actual value of height*/

 fputint(  259,fp,2); /*Tag for compression setting*/
 fputint(    3,fp,2); /*3 for SHORT data type.*/
 fputint(    1,fp,4); /*count is 1*/
 fputint(1,fp,4);     /*1 for uncompressed*/

 fputint(  262,fp,2); /*Tag for Photometric Interpretation*/
 fputint(    3,fp,2); /*3 for SHORT data type.*/
 fputint(    1,fp,4); /*count is 1*/
 fputint(1,fp,4);     /*0=BlackIsZero or 1=BlackIsZero(my preference)*/

 fputint(  273,fp,2); /*Tag for Strip offsets.*/
 fputint(    4,fp,2); /*4 for LONG data type.*/
 fputint(    1,fp,4); /*count is 1 because there is only one strip in this file.*/
 fputint(Pixels_Address,fp,4);     /*Address where the pixels start.*/

 fputint(  278,fp,2); /*Rows Per Strip*/
 fputint(    3,fp,2); /*3 for SHORT data type.*/
 fputint(    1,fp,4); /*count is 1*/
 fputint(height,fp,4);/*There are height rows per strip because we put it all in 1 strip.*/

 fputint(  279,fp,2); /*Strip Byte Count*/
 fputint(    4,fp,2); /*4 for LONG data type.*/
 fputint(    1,fp,4); /*count is 1*/
 fputint(IFD_Address-Pixels_Address,fp,4); /*Difference between IFD and Pixel addresses tells us the number of bytes!*/

 x=(ftell(fp)-IFD_Address-2)/12; /*save the count of entries for later.*/
 printf("Number of entries: %d\n",x);

 fputint(0,fp,4); /*Finish the IFD with 4 zero bytes.*/

 fseek(fp,0x4,SEEK_SET); /*Seek back to address 4 which stores the pointer of the IFD.*/
 fputint(IFD_Address,fp,4); /*write the address of the IFD*/

 fseek(fp,IFD_Address,SEEK_SET); /*Seek to IFD address*/
 fputint(x,fp,2); /*write the number of entries calculated earlier*/

 fclose(fp);
}


/*
Function to make 8 bit grayscale TIFF files! It's a copy of the monochrome one with only two changes. I changed the algorithm for the pixel data to grayscale and tag 258 with a value of 8 lets any program reading the image that it is 8 bit grayscale. All other tags are the same.
*/

void BBM_SaveTIF_Gray(uint32_t *p,uint32_t width,uint32_t height,const char *filename)
{
 uint32_t x,y,pixel,IFD_Address,Pixels_Address,r,g,b,gray;
 FILE* fp;
 fp=fopen(filename,"wb+");
 if(fp==NULL){printf("Failed to create file \"%s\".\n",filename); return;}
 else{printf("File \"%s\" opened for writing.\n",filename);}
 
 fwrite(TIFF_Header,sizeof(char),sizeof(TIFF_Header),fp);

 Pixels_Address=ftell(fp);

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

 IFD_Address=ftell(fp); /*IFD begins after pixel data. Get the current location.*/

 fputint(0,fp,2); /*write two zero bytes. Later on we will replace this with the number of entries in this IFD*/

 /*Each IFD entry is exactly 12 bytes.
  2 for the magic number identifier.
  2 for the data type
  4 for the count(most tags are a count of 1 but there are exceptions).
  4 for the value(or pointer to data elsewhere in the case of an array)
 */
 fputint(  256,fp,2); /*Tag for image width*/
 fputint(    3,fp,2); /*3 for SHORT data type. See page 15 of TIFF 6.0 documentation.*/
 fputint(    1,fp,4); /*count is 1*/
 fputint(width,fp,4); /*Actual value of width*/

 fputint(  257,fp,2); /*Tag for image height*/
 fputint(    3,fp,2); /*3 for SHORT data type. See page 15 of TIFF 6.0 documentation.*/
 fputint(    1,fp,4); /*count is 1*/
 fputint(height,fp,4); /*Actual value of height*/
 
 /*this next tag is the one that determines that this is grayscale rather than monochrome.*/
 fputint(  258,fp,2); /*Tag for bits per sample.*/
 fputint(    3,fp,2); /*3 for SHORT data type.*/
 fputint(    1,fp,4); /*count is 1*/
 fputint(8,fp,4);     /*8 bits per sample of Grayscale*/

 fputint(  259,fp,2); /*Tag for compression setting*/
 fputint(    3,fp,2); /*3 for SHORT data type.*/
 fputint(    1,fp,4); /*count is 1*/
 fputint(1,fp,4);     /*1 for uncompressed*/

 fputint(  262,fp,2); /*Tag for Photometric Interpretation*/
 fputint(    3,fp,2); /*3 for SHORT data type.*/
 fputint(    1,fp,4); /*count is 1*/
 fputint(1,fp,4);     /*0=BlackIsZero or 1=BlackIsZero(my preference)*/

 fputint(  273,fp,2); /*Tag for Strip offsets.*/
 fputint(    4,fp,2); /*4 for LONG data type.*/
 fputint(    1,fp,4); /*count is 1 because there is only one strip in this file.*/
 fputint(Pixels_Address,fp,4);     /*Address where the pixels start.*/

 fputint(  278,fp,2); /*Rows Per Strip*/
 fputint(    3,fp,2); /*3 for SHORT data type.*/
 fputint(    1,fp,4); /*count is 1*/
 fputint(height,fp,4);/*There are height rows per strip because we put it all in 1 strip.*/

 fputint(  279,fp,2); /*Strip Byte Count*/
 fputint(    4,fp,2); /*4 for LONG data type.*/
 fputint(    1,fp,4); /*count is 1*/
 fputint(IFD_Address-Pixels_Address,fp,4); /*Difference between IFD and Pixel addresses tells us the number of bytes!*/

 x=(ftell(fp)-IFD_Address-2)/12; /*save the count of entries for later.*/
 printf("Number of entries: %d\n",x);

 fputint(0,fp,4); /*Finish the IFD with 4 zero bytes.*/

 fseek(fp,0x4,SEEK_SET); /*Seek back to address 4 which stores the pointer of the IFD.*/
 fputint(IFD_Address,fp,4); /*write the address of the IFD*/

 fseek(fp,IFD_Address,SEEK_SET); /*Seek to IFD address*/
 fputint(x,fp,2); /*write the number of entries calculated earlier*/

 fclose(fp);
}



/*
Function to make full 24 bit RGB TIFF files!

To get a test image in development I used this command.

magick o/bbm.bmp o/bbm.tiff


https://imagemagick.org/script/formats.php

*/

void BBM_SaveTIF_RGB(uint32_t *p,uint32_t width,uint32_t height,const char *filename)
{
 uint32_t x,y,pixel,IFD_Address,Pixels_Address,r,g,b;
 FILE* fp;
 fp=fopen(filename,"wb+");
 if(fp==NULL){printf("Failed to create file \"%s\".\n",filename); return;}
 else{printf("File \"%s\" opened for writing.\n",filename);}
 
 fwrite(TIFF_Header,sizeof(char),sizeof(TIFF_Header),fp);

 Pixels_Address=ftell(fp);

 /*write the pixels in R,G,B order*/
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



 IFD_Address=ftell(fp); /*IFD begins after pixel data. Get the current location.*/

 fputint(0,fp,2); /*write two zero bytes. Later on we will replace this with the number of entries in this IFD*/

 /*Each IFD entry is exactly 12 bytes.
  2 for the magic number identifier.
  2 for the data type
  4 for the count(most tags are a count of 1 but there are exceptions).
  4 for the value(or pointer to data elsewhere in the case of an array)
 */
 fputint(  256,fp,2); /*Tag for image width*/
 fputint(    3,fp,2); /*3 for SHORT data type. See page 15 of TIFF 6.0 documentation.*/
 fputint(    1,fp,4); /*count is 1*/
 fputint(width,fp,4); /*Actual value of width*/

 fputint(  257,fp,2); /*Tag for image height*/
 fputint(    3,fp,2); /*3 for SHORT data type. See page 15 of TIFF 6.0 documentation.*/
 fputint(    1,fp,4); /*count is 1*/
 fputint(height,fp,4); /*Actual value of height*/

 fputint(  258,fp,2); /*Tag for bits per sample. 8 for RGB images.*/
 fputint(    3,fp,2); /*3 for SHORT data type.*/
 fputint(    1,fp,4); /*count is 1*/
 fputint(8,fp,4);     /*8 bits per sample of each Red, Green, and Blue*/


 fputint(  259,fp,2); /*Tag for compression setting*/
 fputint(    3,fp,2); /*3 for SHORT data type.*/
 fputint(    1,fp,4); /*count is 1*/
 fputint(1,fp,4);     /*1 for uncompressed*/

 fputint(  262,fp,2); /*Tag for Photometric Interpretation*/
 fputint(    3,fp,2); /*3 for SHORT data type.*/
 fputint(    1,fp,4); /*count is 1*/
 fputint(2,fp,4);     /*2 for RGB*/


 fputint(  273,fp,2); /*Tag for Strip offsets.*/
 fputint(    4,fp,2); /*3 for LONG data type.*/
 fputint(    1,fp,4); /*count is 1 because there is only one strip in this file.*/
 fputint(Pixels_Address,fp,4);     /*Address where the pixels start.*/

 /*this next tag is a new one required for rgb images*/
 fputint(  277,fp,2); /*Tag for Samples Per Pixel*/
 fputint(    3,fp,2); /*3 for SHORT data type.*/
 fputint(    1,fp,4); /*count is 1*/
 fputint(3,fp,4);     /*3=RGB*/

 fputint(  278,fp,2); /*Rows Per Strip*/
 fputint(    3,fp,2); /*3 for SHORT data type.*/
 fputint(    1,fp,4); /*count is 1*/
 fputint(height,fp,4);/*There are height rows per strip because we put it all in 1 strip.*/

 fputint(  279,fp,2); /*Strip Byte Count*/
 fputint(    3,fp,2); /*3 for SHORT data type.*/
 fputint(    1,fp,4); /*count is 1*/
 fputint(IFD_Address-Pixels_Address,fp,4); /*Difference between IFD and Pixel addresses tells us the number of bytes!*/

 x=(ftell(fp)-IFD_Address-2)/12; /*save the count of entries for later.*/
 printf("Number of entries: %d\n",x);

 fputint(0,fp,4); /*Finish the IFD with 4 zero bytes.*/

 fseek(fp,0x4,SEEK_SET); /*Seek back to address 4 which stores the pointer of the IFD.*/
 fputint(IFD_Address,fp,4); /*write the address of the IFD*/

 fseek(fp,IFD_Address,SEEK_SET); /*Seek to IFD address*/
 fputint(x,fp,2); /*write the number of entries calculated earlier*/

 fclose(fp);
}