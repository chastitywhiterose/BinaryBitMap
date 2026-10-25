/*
I moved several things into this new header file. This header has functions for generating simple patterns. The checkerboard was the first but there will be more if I find time. The functions in here usually operate on an already existing array of pixels, usually pointed to by a pointer named p. The width and height are also passed to every function so it knows how to access the array at the correct place and not have a huge memory access error. This method works but I will use a different method when I do a C++ version of my BBM library.
*/

/*
This function is a copy of the original source for Bresenham's line algorithm. I made this copy so I could experiment by changing parts of it without losing the original. Of course I don't intend to change what it does but I have a certain way I like the code to look.

I also made portability changes in case I need to use the algorithm in another programming language. The syntax and operators of most languages are the same with minor differences in how operators are used and in the names of function calls. Having code which operates in a subset of more than one language sames me time in the future. C is my main language but I like others too.

I removed the calls to the abs function and used conditionals to negate things that go below zero. This helps if translating to languages which don't have the abs functions or might be named something weird like "math.abs" .

I also replaced the statements with the ? operator because that operator means something entirely different in Rust than it does in C or C++.
*/
void bbm_line(uint32_t* p,int width,int height,uint32_t color,int x0,int y0,int x1,int y1)
{
 int dx,dy,sx,sy,err,e2;
 dx= x1-x0; if(dx<0){dx=-dx;}
 dy= y1-y0; if(dy<0){dy=-dy;}
 if(x0<x1){sx=1;}else{sx=-1;}
 if(y0<y1){sy=1;}else{sy=-1;}
 if(dx>dy){err=dx;}else{err=-dy;}err>>=1;;
 while(1)
 {
  p[x0+y0*width]=color;
  if (x0==x1 && y0==y1) break;
  e2=err;
  if (e2 >-dx) { err -= dy; x0 += sx; }
  if (e2 < dy) { err += dx; y0 += sy; }
 }
}


/*
Fills the whole image with a single color.
*/
void BBM_Fill(uint32_t *p,uint32_t width,uint32_t height,uint32_t color)
{
 uint32_t x,y;
 y=0;
 while(y<height)
 {
  x=0;
  while(x<width)
  {
   p[x+y*width]=color;
   x+=1;
  }
  y+=1;
 }
}

/*
XORs the whole image with a single color.
*/
void BBM_XOR(uint32_t *p,int width,int height,int color)
{
 int x,y;
 y=0;
 while(y<height)
 {
  x=0;
  while(x<width)
  {
   p[x+y*width]^=color;
   x+=1;
  }
  y+=1;
 }
}

/*
ANDs the whole image with a single color.
*/
void BBM_AND(uint32_t *p,int width,int height,int color)
{
 int x,y;
 y=0;
 while(y<height)
 {
  x=0;
  while(x<width)
  {
   p[x+y*width]&=color;
   x+=1;
  }
  y+=1;
 }
}

/*
makes a square tunnel with squares that start out as filling the whole image and get smaller
It uses the global 2 color index the same as the checkerboard functions do.
*/
void chastity_square_tunnel(uint32_t *p,int width,int height,int square_size)
{
 int x,y,x1=0,y1=0,x2=width,y2=height,index=0;
 while(y1<y2)
 {
  y=y1;
  while(y<y2)
  {
   x=x1;
   while(x<x2)
   {
    p[x+y*width]=u32bw[index];
    x+=1;
   }
   y+=1;
  }
  x1+=square_size;
  y1+=square_size;
  x2-=square_size;
  y2-=square_size;
  index^=1;
 }
}











/*
Code for filling the image with a checkerboard. This is my most precious of programming creations!
It's old and hasn't been updated because every example in my BBM project calls it. If I change anything the world is destroyed!
*/
void chastity_checker(uint32_t *p,uint32_t width,uint32_t height,uint32_t square_size)
{
 uint32_t x,y=0,index=0,index1,bitcountx,bitcounty=0;
 while(y<height)
 {
  index1=index;
  bitcountx=0;
  x=0;
  while(x<width)
  {
   p[x+y*width]=u32bw[index];
   bitcountx+=1;if(bitcountx==square_size){bitcountx=0;index^=1;}
   x+=1;
  }
  index=index1;
  bitcounty+=1;if(bitcounty==square_size){bitcounty=0;index^=1;}
  y+=1;
 }
 printf("Created Checker Pattern with Square Size %d\n",square_size);
}



/*
The following is a modified chastity_checker function which draws parallelograms instead of squares.
*/

void chastity_checker_parallelogram(uint32_t *p,int width,int height,int square_size)
{
 int i=0,x,y=0,index=0,index1,bitcountx,bitcountx1;
 bitcountx=0;
 while(y<height)
 {
  index1=index;
  bitcountx1=bitcountx;
  x=0;
  while(x<width)
  {
   p[i]=u32bw[index]; i+=1;
   bitcountx+=1;if(bitcountx==square_size){bitcountx=0;index^=1;}
   x+=1;
  }
  index=index1;
  bitcountx=bitcountx1;
  bitcountx+=1;
  if(bitcountx==square_size){bitcountx=0;}

  y+=1;
 }
 printf("Created parallelogram Checker Pattern with Square Size %d\n",square_size);
}


/*
Below is a function pointer to allow calling the chastity_checker function by a different name.
It is usually commented out because I don't use it, but maybe I'll use it someday for something.
void (*Chastity_Checker)(uint32_t*,int,int,int)=&chastity_checker;
*/










/*
The long awaited update to the "chastity_checker" function. This uses a different system and requires a LOT more arguments.
In addition to the 4 standard variables from the original function:

          p: pointer to the pixels
      width: horizontal size of the entire image
     height: vertical size of the entire image
square_size: the width and height of each individual square of the checkerboard

It also requires 4 new variables

upper_left_x: x position of top left corner of the selection rectangle
upper_left_y: y position of top left corner of the selection rectangle
  area_width:  width of selection rectangle
 area_height: height of selection rectangle

This function is powerful because it allows drawing a checkerboard ONLY to a part of an image instead of the whole thing at once. The cost of this is that it's just so complicated that only I know how to use it right. It's also image specific and will crash if the selection goes outside of the image boundaries. Use with caution.

here is an example of calling this function:

 BBM_checker(p,width,height,square_size,0,height-square_size*6,width,square_size*6);

Those variables must be defined withing the scope of the calling function, which is usually the case in my own example functions.
*/

void BBM_checker(uint32_t *p,int width,int height,int square_size,
                 int upper_left_x,int upper_left_y,int area_width,int area_height)
{
 int x,y,index=0,index1,bitcountx,bitcounty=0;
 int lower_right_x=upper_left_x+area_width,lower_right_y=upper_left_y+area_height;

 y=upper_left_y;
 while(y<lower_right_y)
 {
  index1=index;
  bitcountx=0;
  x=upper_left_x;
  while(x<lower_right_x)
  {
   p[x+y*width]=u32bw[index];
   bitcountx+=1;if(bitcountx==square_size){bitcountx=0;index^=1;}
   x+=1;
  }
  index=index1;
  bitcounty+=1;if(bitcounty==square_size){bitcounty=0;index^=1;}
  y+=1;
 }
}


/*
 this function is used for copying an image to part of another one. It currently is written to copy an entire image.
 Usually this is a smaller image copied to a part of a larger one.
*/
void BBM_copy
(
 uint32_t *pointer,uint32_t width,uint32_t height,
 uint32_t *source_pointer,uint32_t source_width,uint32_t source_height,
 uint32_t x,uint32_t y
)
{
 uint32_t x1,y1,x2,y2;

 x1=0;
 y1=0;
 x2=x1+source_width;
 y2=y1+source_height;
 while(y1<y2)
 {
  while(x1<x2)
  {
   pointer[x+y*width]=source_pointer[x1+y1*source_width];
   x+=1;
   x1+=1;
  }
  x-=source_width;
  x1-=source_width;

  y+=1;
  y1+=1;
 }

}


















/*
 Clever method to make Diagonal stripes from existing checkerboard.
*/
void chastity_stripes_diagonal(uint32_t *p,int width,int height,int square_size)
{
 int x,y,pixel;
 /*first call the checkerboard function to give us an initial pattern to work with.*/
 chastity_checker(p,width,height,square_size);

 y=1;
 while(y<height)
 {
  x=0;
  pixel=p[x+(y-1)*width]; /*Get the leftmost pixel at from the row above*/
  while(x<(width-1))
  {
   p[x+y*width]=p[(x+1)+(y-1)*width]; /*Copy pixel from the one above and to the right.*/
   x+=1;
  }
  p[x+y*width]=pixel; /*Copy the pixel from earlier to the rightmost pixel of this row.*/
  y+=1;
 }

}

/*
 Clever method to make horizontal stripes from existing checkerboard.
*/
void chastity_stripes_horizontal(uint32_t *p,int width,int height,int square_size)
{
 int x,y;
 /*first call the checkerboard function to give us an initial pattern to work with.*/
 chastity_checker(p,width,height,square_size);
 y=0;
 while(y<height)
 {
  x=1; /*Start at second column*/
  while(x<width)
  {
   p[x+y*width]=p[(x-1)+y*width]; /*Copy pixel from the one left of it.*/
   x+=1;
  }
  y+=1;
 }
}




/*
 Clever method to make vertical stripes from existing checkerboard.
*/
void chastity_stripes_vertical(uint32_t *p,int width,int height,int square_size)
{
 int x,y;
 /*first call the checkerboard function to give us an initial pattern to work with.*/
 chastity_checker(p,width,height,square_size);
 y=1; /*Start at second row*/
 while(y<height)
 {
  x=0;
  while(x<width)
  {
   p[x+y*width]=p[x+(y-1)*width]; /*Copy pixel from the one above it.*/
   x+=1;
  }
  y+=1;
 }
}












/*
Function that can draw a rectangle of any size and color into my pixel array! I used this function in my gingham function because I have to be very specific of where I draw the squares.
 */
void bbm_rect(uint32_t* pixels,int width,int height,int px,int py,int rect_size_x,int rect_size_y,uint32_t color)
{
 int x,y;
 int px2=px+rect_size_x;
 int py2=py+rect_size_y;

 if(px2>width){px2=width; /*printf("px2=%d\n",px2);*/}
 if(py2>height){py2=height; /*printf("py2=%d\n",py2);*/}

 y=py;
 while(y<py2)
 {
  x=px;
  while(x<px2)
  {
   pixels[x+y*width]=color;
   x+=1;
  }
  y+=1;
 }
}

/*my special gingham routine which doesn't actually draw gray squares but leave the background where they would have been*/
void chastity_gingham(uint32_t* p,int width,int height,int square_size)
{
 int index=0,x,y;
 /*first must draw the checkerboard of 1 square size for the gray illusion.*/
 chastity_checker(p,width,height,1);
 y = 0;
 while(y<height)
 {
  int index1=index;
  x=0;
  while(x<width)
  {
   if(index1==0)
   {
    if(index==0){bbm_rect(p,width,height,x,y,square_size,square_size,u32bw[0]);}
   }
   if(index1==1)
   {
    if(index==0){bbm_rect(p,width,height,x,y,square_size,square_size,u32bw[1]);}
   }
   index^=1; x+=square_size;
  }
  index=index1^1; y+=square_size;
 }
 printf("Created Gingham Pattern with Square Size %d\n",square_size);
}




/*
 The next 4 functions are actually animation related but it was time to make them part of this main BBM library after fixing some bugs and adding features. I kept the out of date ones in bbm_gif.h until I decide what to do with them.
*/

/*roll all pixels to the left*/
void BBM_roll_full_left(uint32_t *p,uint32_t width,uint32_t height)
{
 uint32_t x,y,t;
 y=0;
 while(y<height)
 {
  x=0;
  t=p[x+y*width];
  while(x<width-1)
  {
   p[x+y*width]=p[(x+1)+y*width];
   x+=1;
  }
  p[x+y*width]=t;
  y+=1;
 }
}

/*roll all pixels to the right*/
void BBM_roll_full_right(uint32_t *p,uint32_t width,uint32_t height)
{
 uint32_t x,y,t;
 y=0;
 while(y<height)
 {
  x=width-1;
  t=p[x+y*width];
  while(x>1)
  {
   p[x+y*width]=p[(x-1)+y*width];
   x--;
  }
  p[x+y*width]=t;
  y+=1;
 }
}





/*roll all pixels up*/
void BBM_roll_full_up(uint32_t *p,uint32_t width,uint32_t height)
{
 uint32_t x,y,t;
 x=0;
 while(x<width)
 {
  y=0;
  t=p[x+y*width];
  while(y<height-1)
  {
   p[x+y*width]=p[x+ (y+1) *width];
   y+=1;
  }
  p[x+y*width]=t;
  x+=1;
 }
}



/*roll all pixels down*/
void BBM_roll_full_down(uint32_t *p,uint32_t width,uint32_t height)
{
 uint32_t x,y,t;

 x=0;
 while(x<width)
 {
  y=height-1;
  t=p[x+y*width];
  while(y>0)
  {
   p[x+y*width]=p[x+(y-1)*width];
   y--;
  }
  p[x+y*width]=t;
  x+=1;
 }
}



/*next the functions that require additional arguments to specify the rectangular area to be rolled.*/


/*roll some pixels to the left*/
void BBM_roll_part_left(uint32_t *p,int width,int height,uint32_t upper_left_x,uint32_t upper_left_y,int area_width,int area_height)
{
 uint32_t x,y,t;
 uint32_t lower_right_x=upper_left_x+area_width,lower_right_y=upper_left_y+area_height;
 y=upper_left_y;
 while(y<lower_right_y)
 {
  x=upper_left_x;
  t=p[x+y*width];
  while(x<lower_right_x-1)
  {
   p[x+y*width]=p[(x+1)+y*width];
   x+=1;
  }
  p[x+y*width]=t;
  y+=1;
 }
}


/*roll some pixels to the right*/
void BBM_roll_part_right(uint32_t *p,int width,int height,uint32_t upper_left_x,uint32_t upper_left_y,int area_width,int area_height)
{
 uint32_t x,y,t;
 uint32_t lower_right_x=upper_left_x+area_width,lower_right_y=upper_left_y+area_height;
 y=upper_left_y;
 while(y<lower_right_y)
 {
  x=lower_right_x-1;
  t=p[x+y*width];
  while(x>upper_left_x)
  {
   p[x+y*width]=p[(x-1)+y*width];
   x--;
  }
  p[x+y*width]=t;
  y+=1;
 }
}


/*roll some pixels up*/
void BBM_roll_part_up(uint32_t *p,int width,int height,uint32_t upper_left_x,uint32_t upper_left_y,int area_width,int area_height)
{
 uint32_t x,y,t;
 uint32_t lower_right_x=upper_left_x+area_width,lower_right_y=upper_left_y+area_height;
 x=upper_left_x;
 while(x<lower_right_x)
 {
  y=upper_left_y;
  t=p[x+y*width];
  while(y<lower_right_y-1)
  {
   p[x+y*width]=p[x+ (y+1) *width];
   y+=1;
  }
  p[x+y*width]=t;
  x+=1;
 }
}


/*roll some pixels down*/
void BBM_roll_part_down(uint32_t *p,int width,int height,uint32_t upper_left_x,uint32_t upper_left_y,int area_width,int area_height)
{
 uint32_t x,y,t;
 uint32_t lower_right_x=upper_left_x+area_width,lower_right_y=upper_left_y+area_height;
 x=upper_left_x;
 while(x<lower_right_x)
 {
  y=lower_right_y-1;
  t=p[x+y*width];
  while(y>upper_left_y)
  {
   p[x+y*width]=p[x+(y-1)*width];
   y--;
  }
  p[x+y*width]=t;
  x+=1;
 }
}


/*
 I know this function looks complex but it was built in small stages very carefully.

What it does is convert the colors in an existing image to the same colors they would be in if they were a different bit depth.
It can do 1,2,4,8 bit grayscale or 3,6,12,24 bit Red Green Blue. The bits per pixel or "bpp" determines which algorithm is used to modify the colors.

The reason I decided to do this function was to be able to view what these look like quickly with just one function. The old method required creating different pbm, pgm, and ppm files which were converted to PNG files before I could see them. This is so much faster!
*/

void BBM_Set_BPP(uint32_t *p,uint32_t width,uint32_t height,int bpp)
{
 uint32_t x,y,pixel,r,g,b,gray,bitcount;

 /* if bpp is one of these, use the grayscale conversion. */
 if(bpp==1||bpp==2||bpp==4||bpp==8)
 {
  y=0;
  while(y<height)
  {
   x=0;
   while(x<width)
   {
    /*First, get the pixel and extract each color component.*/
    pixel=p[x+y*width];
    r=(pixel&0xFF0000)>>16;
    g=(pixel&0x00FF00)>>8;
    b=(pixel&0x0000FF);
    gray=(r+g+b)/3;
    gray>>=8-bpp;

    /*convert gray into a 24 bit RGB equivalent.*/
    pixel=0;
    bitcount=0;
    while(bitcount<24)
    {
     pixel<<=bpp;
     pixel|=gray;
     bitcount+=bpp;
    }

    p[x+y*width]=pixel;
    x+=1;
   }
   y+=1;
  }
 }

 else if(bpp==3||bpp==6||bpp==12||bpp==24)
 {
  bpp/=3; /*divide the bpp by 3 because each component is a third of the size of the total bpp*/

  y=0;
  while(y<height)
  {
   x=0;
   while(x<width)
   {
    /*First, get the pixel and extract each color component.*/
    pixel=p[x+y*width];
    r=(pixel&0xFF0000)>>16;
    g=(pixel&0x00FF00)>>8;
    b=(pixel&0x0000FF);

    /*right shift each component by the bpp to eliminate extra bits.*/
    r>>=8-bpp;
    g>>=8-bpp;
    b>>=8-bpp;

    /*next left shift red and green back to their proper position. blue doesn't need to be shifted.*/
    r<<=16;
    g<<=8;

    /*
     The following loop puts the
     components back into the pixel which is always represented as
     24 bit rgb in all my functions.
    */   

    pixel=0;
    bitcount=0;
    while(bitcount<8)
    {
     pixel<<=bpp;
     pixel|=r;
     pixel|=g;
     pixel|=b;
     bitcount+=bpp;
    }

    p[x+y*width]=pixel;

    x+=1;
   }
   y+=1;
  }
  
 }

 else{printf("bpp of %d not supported.\n",bpp);}

}








/*
Bresenham's line algorithm

Edited Slightly by Chastity White Rose.

Info about algorithm from here:
https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm#Algorithm_for_integer_arithmetic

Code from here.
https://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm#C

*/
void bbm_line_Bresenham(uint32_t* p,int width,int height,uint32_t color,int x0,int y0,int x1,int y1)
{
 int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
 int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1; 
 int err = (dx>dy ? dx : -dy)/2, e2;
 for(;;)
 {
  p[x0+y0*width]=color;
  if (x0==x1 && y0==y1) break;
  e2 = err;
  if (e2 >-dx) { err -= dy; x0 += sx; }
  if (e2 < dy) { err += dx; y0 += sy; }
 }
}



/*
This function was a lot of work but it allows basic scaling of images. For example a tiny image can be magnified so I can see the individual pixels as large squares.

pointer=original pointer to pixels in source image
width=width of source image
height=height of source image

pointer1=pointer to pixels in target image
widthscale=widthscale of target image
heightscale=heightscale of target image
*/
void bbm_scale(uint32_t *pointer,uint32_t width,uint32_t height,uint32_t *pointer1,uint32_t widthscale,uint32_t heightscale)
{
 uint32_t x,y,color;   /*used in source image*/
 uint32_t x1,y1,x2,y2; /*used in target image*/
 uint32_t width1=width*widthscale;
 /*uint32_t height1=height*heightscale;*/

 y=0;
 while(y<height)
 {
  x=0;
  while(x<width)
  {
   /*get color of current pixel*/
   color=pointer[x+y*width];
   /*start another loop to draw a rectangle*/
   y1=y*heightscale;
   y2=(y+1)*heightscale;
   while(y1<y2)
   {
    x1=x*widthscale;
    x2=(x+1)*widthscale;
    while(x1<x2)
    {
     pointer1[x1+y1*width1]=color;
     x1+=1;
    }
    y1+=1;
   }


   x+=1;
  }
  y+=1;
 }
}
