/*
This is the header file which contains my new and improved C++ class as well as several global variables which are used for storing the points of regular polygons and a buffer of bytes which is modified and written to a file in the functions that save image files.
*/

/*Some of my code after here makes heavy use of a static array and variables to keep track of the number of bytes used.*/
#define array_length_max 0x1000000
uint8_t array[array_length_max]; //shared byte array of all objects and functions
uint32_t array_length; //this length will be modified by all file IO functions.
uint32_t array_index; //the current location of the array where reading or writing will take place.

/*
This function writes the passed integer and size in bytes of the integer to the current index based on the array_index variable above.
It is a speed optimization to do all the writing into the memory buffer so that I only need to fwrite the array once into an open file for all of my file output functions. It's an improvement over the fputint and fputc method I used in my C code. However, there is nothing about this that specifically requires C++. It may be used in the future in my C code.
*/
void aputint(unsigned long int i,int size)
{
 while(size>0)
 {
  array[array_index]=i;
  array_index+=1;
  i>>=8;
  size--;
 }
}


/*The reverse process gets an integer of the specified number of bytes in little endian format.*/
unsigned long int agetint(int size)
{
 unsigned long int i=0,c,x;
 
 x=0;
 while(size>0)
 {
  c=array[array_index];
  array_index+=1;
  i+=c<<x;
  x+=8;
  size--;
 }

 return i;
}

/*
a function to load an entire file into the global byte array and keep track of the size of it.
*/
void load_file(const char* filename)
{
 FILE* fp;
 fp=fopen(filename,"rb");
 if(fp==NULL){printf("Failed to open file \"%s\" for reading.\n",filename); return;}
 else{printf("File \"%s\" opened.\n",filename);}
 array_length=fread(array,1,array_length_max,fp);
 printf("Length of file is %d\n",array_length);
 fclose(fp);
}



/*animation variables*/
int frame=0; /*current frame number of animation. Starts at 0.*/
char filename[256]; /*global file name which is used in the save_frame function.*/
int pixel_per_frame=0; /*if nonzero, save a frame each time the put_pixel function is called.*/


/*global polygon related data and functions begin here*/

/*returns nonzero if double is less than zero. Required for rounding function.*/
double bbm_signbit(double x)
{
 return x<0;
}

/*
This function was needed to correctly convert the floating point coordinates to the nearest integer.
I got the source from here:
https://en.cppreference.com/w/c/numeric/math/round
*/
double round(double x)
{
 return bbm_signbit(x) ? ceil(x - 0.5) : floor(x + 0.5);;
}

double radians=0; /*the rotation in radians measurement of the polygon.*/
int xpoints[0x1000],ypoints[0x1000]; /*global arrays for x and y coordinates.*/

 /*Define PI because it is required to use sine and cosine functions*/
#define M_PI 3.14159265358979323846

/*
The function that stores the points of the corners of a regular polygon into the arrays above.
This was the final and most advanced part of writing my BBM library. It is also the only place that the math.h header is required and where floating point values are used. After the complex formula that determines where the corners of a polygon based on the variables passed, it stores them into the global arrays. This is because it would have been far too complicated to pass the arrays as arguments to every function and also because once a polygon is drawn, it no longer matters if they are rewritten. New polygons can be drawn at any time by using the
*/
void poly_points(double cx,double cy,double radius,double radians,int points)
{
 int i=0;
 double f,angle,x,y;
 /*printf("Making points for Regular Polygon with %d points\n",points);*/
 i=0;
 while(i<points)
 {
  f=(double)i/points;
  angle=2*M_PI*f;
  angle+=radians;
  x=sin(angle);
  y=cos(angle);
  x=cx+x*radius;
  y=cy-y*radius;  /*printf("x=%f,y=%f\n",x,y);*/
  x=round(x);
  y=round(y);
  xpoints[i]=x;
  ypoints[i]=y;
  i++;
 }
}




class BinaryBitMap
{
 public:
 uint32_t width,height,length,bitsperpixel,*pointer,x,y;

 
 BinaryBitMap()
 {
  printf("This is the default constructor\n");
  width=512;height=512;
  alloc();
  bitsperpixel=24;
 }

 BinaryBitMap(int w,int h)
 {
  printf("This is the custom constructor\n");
  width=w;height=h;
  alloc();
  bitsperpixel=24;
 }

void alloc() /*allocates memory based on the length variable*/
{
 length=width*height;
 pointer=(uint32_t*)malloc(length*sizeof(*pointer));
 if(pointer==NULL){printf("pixel malloc failed\n");}
 //printf("pixels allocated at address: %p\n",(void*)pointer);

}

 void load_bmp(const char* filename)
 {
  int dib,new_width,new_height,bpp,data,pixel,r,g,b,bitcount;

  load_file(filename); /*first load the whole file into global array*/

  array_index=0xA;
  data=agetint(4); /*read 4 byte integer*/
  printf("pixel data at==%d\n",data);
  array_index=0xE;
  dib=agetint(4); /*read 4 byte integer*/
  printf("dib==%d\n",dib);
  if(dib==40||dib==124)
  {
   array_index=0x12;
   new_width=agetint(4);
   new_height=agetint(4);
   printf("new_width==%d\n",new_width);
   printf("new_height==%d\n",new_height);
   array_index=0x1C;
   bpp=agetint(2);
   printf("bpp==%d\n",bpp);


   if(bpp==24)
   {
    printf("Type of image is 24 bpp or full color RGB\n");
    width=new_width;
    height=new_height;

    array_index=data; /*go to location where the pixel data begins*/
    
    y=height;
    while(y>0)
    {
     y--;
     bitcount=0;
     x=0;
     while(x<width)
     {
      pixel=0;
      b=agetint(1);
      g=agetint(1);
      r=agetint(1);
      pixel|=r<<16;
      pixel|=g<<8;
      pixel|=b;
      pointer[x+y*width]=pixel;
      x++;

      bitcount+=bpp;

     }

     /*if pixel row is not multiple of 4 bytes, this skips the extra padding bytes*/
     while(bitcount%32!=0)
     {
      array_index+=1;
      bitcount+=8;
     }


    }

   }



   if(bpp==32)
   {
    printf("Type of image is 32 bpp or full color RGB\n");
    width=new_width;
    height=new_height;

    array_index=data; /*go to location where the pixel data begins*/
    
    y=height;
    while(y>0)
    {
     y--;
     x=0;
     while(x<width)
     {
      pixel=0;
      b=agetint(1);
      g=agetint(1);
      r=agetint(1);
      array_index+=1;   /*skip the alpha channel*/
      pixel|=r<<16;
      pixel|=g<<8;
      pixel|=b;
      pointer[x+y*width]=pixel;
      x++;
     }
    }

   }

  }


 }


 ~BinaryBitMap()
 {
  printf("This is the destructor\n");
  if(pointer!=NULL){free(pointer);pointer=NULL;}
 }

 void info()
 {
  printf("Everything you want to know about this bitmap\n");
  printf("width=%d\n",width);
  printf("height=%d\n",height);
  printf("length=%d\n",length);
  printf("bitsperpixel=%d\n",bitsperpixel);
 }




 /*
  Function for drawing a single pixel. I have plans for this one later.
 */
 void put_pixel(int x,int y,uint32_t color)
 {
  pointer[x+y*width]=color;
  //printf("put_pixel(%d,%d,%06X)\n",x,y,color);
  if(pixel_per_frame!=0){save_frame();}
 }

 void line(uint32_t color,int x0,int y0,int x1,int y1)
 {
  int dx,dy,sx,sy,err,e2;
  dx= x1-x0; if(dx<0){dx=-dx;}
  dy= y1-y0; if(dy<0){dy=-dy;}
  if(x0<x1){sx=1;}else{sx=-1;}
  if(y0<y1){sy=1;}else{sy=-1;}
  if(dx>dy){err=dx;}else{err=-dy;}err>>=1;;
  while(1)
  {
   pointer[x0+y0*width]=color;
   if (x0==x1 && y0==y1) break;
   e2=err;
   if (e2 >-dx) { err -= dy; x0 += sx; }
   if (e2 < dy) { err += dx; y0 += sy; }
  }
 }


/*
This function draws a regular polygon using the global poly_points function combined with the member line function to first get the points and then draw the lines with Bresenham's algorithm.
It's named polyline instead of polygon because it draws the lines on the outside but does not fill the inside, which would take much longer.
*/
void polyline(double cx,double cy,double radius,double radians,int points,uint32_t color)
{
 int i,i1;
 poly_points(cx,cy,radius,radians,points);
 i=0;
 while(i<points)
 {
  i1=(i+1)%points;
  line(color,xpoints[i],ypoints[i],xpoints[i1],ypoints[i1]);
  line(color,xpoints[i],ypoints[i],cx,cy);
  i+=1;
 }
}

/*global variables used in the following two functions*/
uint32_t oldcolor,newcolor;

/*
This is step 1 of a new flood fill algorithm. The x and y location of the bitmap is accessed to see what color was there.
The reason for splitting this function into two parts is that this function is called only once to establish which color will be replaced. The part two function will be a recursive function which calls itself.
*/
void flood_fill_part_1(int x,int y,uint32_t color)
{
 oldcolor=pointer[x+y*width];
 newcolor=color;
 printf("oldcolor==%06X\n",oldcolor);
 if(oldcolor==newcolor){return;} /*if old and new colors are identical, cancel the whole thing!*/
 flood_fill_part_2(x,y);
}

/*
This part two of the flood fill is the recursive one.
I got the basic design of this one from these links.
https://rosettacode.org/wiki/Bitmap/Flood_fill#C
http://inventwithpython.com/blog/2011/08/11/recursion-explained-with-the-flood-fill-algorithm-and-zombies-and-cats/

There is a gcc option "-fno-stack-limit" that I will do more research on. It may be needed for my recursive flood fill function.

*/
void flood_fill_part_2(int x,int y)
{
 if(pointer[x+y*width]!=oldcolor){  /*printf("color at %d %d==%06X\n",x,y,pointer[x+y*width]);*/  return;}
 /*printf("color at %d %d==%06X\n",x,y,pointer[x+y*width]);*/
 pointer[x+y*width]=newcolor; //save_frame();
 flood_fill_part_2(x-1,y);
 flood_fill_part_2(x+1,y);
 flood_fill_part_2(x,y-1);
 flood_fill_part_2(x,y+1);
}

/*
This function draws a regular polygon just like bbm_polyline above. However after that it calls another function which attempts to fill in that polygon.
*/
void polygon(double cx,double cy,double radius,double radians,int points,uint32_t color)
{
 int i,i1;
 poly_points(cx,cy,radius,radians,points);
 i=0;
 while(i<points)
 {
  i1=(i+1)%points;
  line(color,xpoints[i],ypoints[i],xpoints[i1],ypoints[i1]);
  i+=1;
 }
 //flood_fill_part_1(cx,cy,color); //flood fill has bug
}



 /*
  This is a scan line fill algorithm. It assumes that there is a single polygon drawn that is a different color than an image otherwise filled with a single color(very specific circumstances). This however works perfectly if the image is first filled with a color and then the polygon drawing function above is used. This simple scan fill only works on certain types of polygons including regular polygons. It's limited but it is faster and doesn't run out of stack space like the other flood fill function did.

The one downside is that to draw polygons onto other images, this secondary image must be created separately and then the two combined somehow. Usually by my XOR method.
 */



 void scan_fill()
 {
  x=0;y=0;
  uint32_t color0=pointer[x+y*width],color1,x1;
  y=0;
  while(y<height)
  {
   x=0;
   while(x<width)
   {  
    /*if find pixel that is different than the background, find boundaries*/
    if(pointer[x+y*width]!=color0) /*left edge*/
    {
     color1=pointer[x+y*width];

     x1=width;
     while(x1>0)
     {
      x1-=1;
      if(pointer[x1+y*width]!=color0){break;} /*right edge*/
     }

     while(x<x1){pointer[x+y*width]=color1;x+=1;} /*fill space between the two on this line*/
    }
    x+=1;
   }
   y+=1;
  }
 }





/*
Totally new method of star polygons. Without the fabulous line drawing algorithm this would never have been possible!
*/
void star(double cx,double cy,double radius,double radians,int points,int step,uint32_t color)
{
 int i,i1,i1_start,i2;
 poly_points(cx,cy,radius,radians,points);
 i=0;
 i1=0;
 i1_start=i1;
 while(i<points)
 {
  i2=(i1+step)%points;
  line(color,xpoints[i1],ypoints[i1],xpoints[i2],ypoints[i2]);
  i1=(i1+step)%points;
  if(i1==i1_start){i1+=1; i1_start+=1;}
  line(color,xpoints[i],ypoints[i],cx,cy);
  i+=1;
 }
}



 /*
  Fills the whole image with a single color.
 */
 void fill(uint32_t color)
 {
  y=0;
  while(y<height)
  {
   x=0;
   while(x<width)
   {
    pointer[x+y*width]=color;
    x+=1;
   }
   y+=1;
  }
 }

/*
Code for filling the image with a checkerboard. This is my most precious of programming creations!
It's old and hasn't been updated because every example in my BBM project calls it. If I change anything the world is destroyed!
*/
void fill_checker(int square_size,uint32_t color0,uint32_t color1)
{
 int index=0,index1,bitcountx,bitcounty=0;
 y=0;
 while(y<height)
 {
  index1=index;
  bitcountx=0;
  x=0;
  while(x<width)
  {
   if(index==0)
   {
    pointer[x+y*width]=color0;
    //put_pixel(x,y,color0);
   }
   else
   {
    pointer[x+y*width]=color1;
    //put_pixel(x,y,color1);
   }
   bitcountx+=1;if(bitcountx==square_size){bitcountx=0;index^=1;}
   x+=1;
  }
  index=index1;
  bitcounty+=1;if(bitcounty==square_size){bitcounty=0;index^=1;}
  y+=1;
 }
 //printf("Created Checker Pattern with Square Size %d\n",square_size);
}












 /*
 A member function which automatically creates a filename based on the global frame number and then calls a saving function.
 This means if I change this, I can control which saving function is called. The 24 bit function allows all colors but makes huge files.
 The 1 bit function can only do black and white but the files are very small which matters greatly the more frames there are.
 */
 void save_frame()
 {
  sprintf(filename,"./o/%08d.bmp",frame);
  printf("%s\n",filename);
  Save_BMP_1(filename);
  frame+=1;
 }


 /*
 the black and white only version of my BMP saving function. My favorite although limited.
 */
 void Save_BMP_1(const char* filename)
 {
  uint32_t red,green,blue,gray,pixel,bits,bpp=1,bitcount;
  FILE* fp;
  fp=fopen(filename,"wb+");
  if(fp==NULL){printf("Failed to create file \"%s\".\n",filename); return;}
  else{/*printf("File \"%s\" opened.\n",filename);*/}

  array_index=0; //the array starts empty, so the current index is 0;
  aputint(0x4D42,0xA);
  aputint(0x20,4); //location where pixels will start
  aputint(12,4);
  aputint(width,2);
  aputint(height,2);
  aputint(1,2); //planes, always 1
  aputint(bpp,2); //1 bit per pixel

  /*Set up the pallete*/
  aputint(0x000000,3);
  aputint(0xFFFFFF,3);

  y=height;
  while(y>0)
  {
   bitcount=0;
   y--;
   x=0;
   while(x<width)
   {
    pixel=pointer[x+y*width];
    red=(pixel&0xFF0000)>>16;
    green=(pixel&0x00FF00)>>8;
    blue=(pixel&0x0000FF);
    gray=(red+green+blue)/3;
    gray>>=8-bpp;
    bits<<=bpp;
    bits|=gray;
    bitcount+=bpp;
    x++;
    if(bitcount%8==0)
    {
     aputint(bits,1);
    }
   }

  /*This loop fixes things when the image is not a multiple of 32 bits in width.*/
  while(bitcount%32!=0)
  {
   bits<<=1;
   bitcount+=bpp;
   if(bitcount%8==0)
   {
    aputint(bits,1);
   }
  }
 }

  array_length=array_index;
  array_index=2; /*go back to address 2*/
  aputint(array_length,4); /*write size of the file*/
  fwrite(array,1,array_length,fp);
  fclose(fp);
 }



 /*
 This saves 3 bits per pixel. Actually each pixel is a single 8 bit byte where each red, green, or blue value uses two bits. The result is a 64 bit palette which is indexed by the bytes. This means that it's about a third of the size of the same image saved in 24 bit mode because each pixel takes only 1 byte instead of 3.
 */
 void Save_BMP_3(const char* filename)
 {
  uint32_t red,green,blue,gray,pixel,bpp=3,bitcount,bits,color;
  FILE* fp;
  fp=fopen(filename,"wb+");
  if(fp==NULL){printf("Failed to create file \"%s\".\n",filename); return;}
  else{/*printf("File \"%s\" opened.\n",filename);*/}

  array_index=0; //the array starts empty, so the current index is 0;
  aputint(0x4D42,0xA);
  aputint(0x32,4); //location where pixels will start
  aputint(12,4);
  aputint(width,2);
  aputint(height,2);
  aputint(1,2); //planes, always 1
  aputint(4,2); //3 bits per pixel are used but it's still 4 according to the format. Each pixel is one half byte.

  /*Set up the pallete*/
  bpp/=3;
  gray=(1<<bpp)-1;


  x=0;
  while(x<8) /*exactly 8 colors in a 3 bit palette.*/
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

   aputint(color,3);

   x++;
  } /*end of palette*/

 y=height;
 while(y>0)
 {
  bitcount=0;
  bits=0;
  y--;
  x=0;
  while(x<width)
  {
   /*First, get the pixel and extract each color component.*/
    pixel=pointer[x+y*width];
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
    bitcount+=4; /*add 4 to bitcount each time a 4 bits is added to the byte.*/


   if(bitcount%8==0) 
   {
    aputint(bits,1); /*every time we have a full byte, add it to the array.*/
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
    aputint(bits,1);
   }
  }


 }



  array_length=array_index;
  array_index=2; /*go back to address 2*/
  aputint(array_length,4); /*write size of the file*/
  fwrite(array,1,array_length,fp);
  fclose(fp);
 }



 /*
 This saves 4 bits per pixel. This is a grayscale function which does 16 shades of gray.
 Each half byte is 1 pixel.
 */
 void Save_BMP_4(const char* filename)
 {
  uint32_t red,green,blue,gray,pixel,bitcount,bits;
  FILE* fp;
  fp=fopen(filename,"wb+");
  if(fp==NULL){printf("Failed to create file \"%s\".\n",filename); return;}
  else{/*printf("File \"%s\" opened.\n",filename);*/}

  array_index=0; //the array starts empty, so the current index is 0;
  aputint(0x4D42,0xA);
  aputint(0x4A,4); //location where pixels will start
  aputint(12,4);
  aputint(width,2);
  aputint(height,2);
  aputint(1,2); //planes, always 1
  aputint(4,2); //4 bits per pixel. Each pixel is one half byte.

  /*Set up the pallete*/
  x=0;
  while(x<256)
  {
   aputint(x,1);
   aputint(x,1);
   aputint(x,1);
   x+=0x11;
  }
  /*end of palette*/

 y=height;
 while(y>0)
 {
  bitcount=0;
  bits=0;
  y--;
  x=0;
  while(x<width)
  {
   /*First, get the pixel and extract each color component.*/
    pixel=pointer[x+y*width];
    red=(pixel&0xFF0000)>>16;
    green=(pixel&0x00FF00)>>8;
    blue=(pixel&0x0000FF);

    gray=(red+green+blue)/3;
    gray>>=4;

    bits<<=4;
    bits|=gray;
    bitcount+=4; /*add 4 to bitcount each time a 4 bits is added to the byte.*/

   if(bitcount%8==0) 
   {
    aputint(bits,1); /*every time we have a full byte, add it to the array.*/
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
    aputint(bits,1);
   }
  }

 }

  array_length=array_index;
  array_index=2; /*go back to address 2*/
  aputint(array_length,4); /*write size of the file*/
  fwrite(array,1,array_length,fp);
  fclose(fp);
 }





 /*
 This saves 6 bits per pixel. Actually each pixel is a single 8 bit byte where each red, green, or blue value uses two bits. The result is a 64 color palette which is indexed by the bytes. This means that it's about a third of the size of the same image saved in 24 bit mode because each pixel takes only 1 byte instead of 3.
 */
 void Save_BMP_6(const char* filename)
 {
  uint32_t red,green,blue,gray,pixel,bpp=6,bitcount,color;
  FILE* fp;
  fp=fopen(filename,"wb+");
  if(fp==NULL){printf("Failed to create file \"%s\".\n",filename); return;}
  else{/*printf("File \"%s\" opened.\n",filename);*/}

  array_index=0; //the array starts empty, so the current index is 0;
  aputint(0x4D42,0xA);
  aputint(0xDA,4); //location where pixels will start
  aputint(12,4);
  aputint(width,2);
  aputint(height,2);
  aputint(1,2); //planes, always 1
  aputint(8,2); //6 bits per pixel are used but it's still 8 according to the format. Each pixel is one byte.

  /*Set up the pallete*/
  bpp/=3;
  gray=(1<<bpp)-1;

  x=0;
  while(x<64) /*exactly 64 colors in a 6 bit palette.*/
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

   aputint(color,3);

   x++;
  } /*end of palette*/

 y=height;
 while(y>0)
 {
  bitcount=0;
  y--;
  x=0;
  while(x<width)
  {
   /*First, get the pixel and extract each color component.*/
    pixel=pointer[x+y*width];
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

    aputint(pixel,1); /*finally output the index to palette array*/
    bitcount+=8; //add 8 to bitcount each time a 1 byte pixel is written

   x++;
  }

  /*This loop fixes things when the image is not a multiple of 32 bits in width.*/
  while(bitcount%32!=0)
  {
   aputint(0,1);
   bitcount+=8;
  }


 }

  array_length=array_index;
  array_index=2; /*go back to address 2*/
  aputint(array_length,4); /*write size of the file*/
  fwrite(array,1,array_length,fp);
  fclose(fp);
 }



 /*
 This saves 8 bits per pixel. Each byte is a grayscale value from 0 to 255.
 */
 void Save_BMP_8(const char* filename)
 {
  uint32_t red,green,blue,gray,pixel,bitcount;
  FILE* fp;
  fp=fopen(filename,"wb+");
  if(fp==NULL){printf("Failed to create file \"%s\".\n",filename); return;}
  else{/*printf("File \"%s\" opened.\n",filename);*/}

  array_index=0; //the array starts empty, so the current index is 0;
  aputint(0x4D42,0xA);
  aputint(0x31A,4); //location where pixels will start
  aputint(12,4);
  aputint(width,2);
  aputint(height,2);
  aputint(1,2); //planes, always 1
  aputint(8,2); //8 bits per pixel. Each pixel is one byte.

  /*Set up the pallete*/
  x=0;
  while(x<256)
  {
   aputint(x,1);
   aputint(x,1);
   aputint(x,1);
   x+=1;
  }
  /*end of palette*/

  y=height;
  while(y>0)
  {
   bitcount=0;
   y--;
   x=0;
   while(x<width)
   {
    /*First, get the pixel and extract each color component.*/
    pixel=pointer[x+y*width];
    red=(pixel&0xFF0000)>>16;
    green=(pixel&0x00FF00)>>8;
    blue=(pixel&0x0000FF);
    gray=(red+green+blue)/3;
    aputint(gray,1);
    bitcount+=8;
    x++;
   }

   /*This loop fixes things when the image is not a multiple of 32 bits in width.*/
   while(bitcount%32!=0)
   {
    aputint(0,1);
    bitcount+=8;
   }
  }

  array_length=array_index;
  array_index=2; /*go back to address 2*/
  aputint(array_length,4); /*write size of the file*/
  fwrite(array,1,array_length,fp);
  fclose(fp);
 }





 // save 24 bpp BMP file.
 void Save_BMP_24(const char* filename)
 {
  int bytecount,pixel;
  FILE* fp;
  fp=fopen(filename,"wb+");
  if(fp==NULL){printf("Failed to create file \"%s\".\n",filename); return;}
  else{printf("File \"%s\" opened.\n",filename);}

  array_index=0; //the array starts empty, so the current index is 0;

  aputint(0x4D42,0xA);
  aputint(0x1A,4); //location where pixels will start
  aputint(12,4);
  aputint(width,2);
  aputint(height,2);
  aputint(1,2); //planes, always 1
  aputint(bitsperpixel,2);


 y=height;
 while(y>0)
 {
  bytecount=0;
  y--;
  x=0;
  while(x<width)
  {
   pixel=pointer[x+y*width];
   aputint(pixel,3);
   bytecount+=3;
   x++;
  }

  while(bytecount%4!=0)
  {
   aputint(0,1);
   bytecount++;
  }
 }

  array_length=array_index;
  array_index=2; /*go back to address 2*/
  aputint(array_length,4); /*write size of the file*/
  fwrite(array,1,array_length,fp);
  fclose(fp);
 }


};