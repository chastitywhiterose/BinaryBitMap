/*
polygon drawing header

This header file is all about creating the points used to make a regular polygon and also possibly use different methods to draw them. By combining the formula for calculating the points of the polygon with the command line tools which create image files, I can draw the polygons to image files and then load them into my programs.
*/

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
double bbm_round(double x)
{
 return bbm_signbit(x) ? ceil(x - 0.5) : floor(x + 0.5);
}



int npoints;
int xpoints[0x1000],ypoints[0x1000];

 /*Define PI because it is required to use sine and cosine functions*/
#define M_PI 3.14159265358979323846

/*
The function that stores the points of the corners of a regular polygon into the arrays above.
This was the final and most advanced part of writing my BBM library. It is also the only place that the math.h header is required and where floating point values are used. After the complex formula that determines where the corners of a polygon based on the variables passed, it stores them into the global arrays. This is because it would have been far to complicated to pass the arrays as arguments to every function and also because once a polygon is drawn, it no longer matters if they are rewritten. New polygons can be drawn at any time by using this function within another function which does the actual drawing.
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
  x=bbm_round(x);
  y=bbm_round(y);
  xpoints[i]=x;
  ypoints[i]=y;
  i++;
 }
 npoints=points;
}


/*
This function draws a regular polygon using the bbm_line function from bbmlib.h . It uses the global arrays for the points.
*/
void bbm_polygon(uint32_t* p,int width,int height,double cx,double cy,double radius,double radians,int points,uint32_t color)
{
 int i,i1;
 poly_points(cx,cy,radius,radians,points);
 i=0;
 while(i<npoints)
 {
  i1=(i+1)%npoints;
  bbm_line(p,width,height,color,xpoints[i],ypoints[i],xpoints[i1],ypoints[i1]);
  /*the next line if not commented out draws lines from center to corners*/
  /*bbm_line(p,width,height,color,xpoints[i],ypoints[i],cx,cy);*/
  i+=1;
 }
}


 /*
  This is a scan line fill algorithm. It assumes that there is a single polygon drawn that is a different color than an image otherwise filled with a single color(very specific circumstances). This however works perfectly if the image is first filled with a color and then the polygon drawing function above is used. This simple scan fill only works on certain types of polygons including regular polygons. It's limited but it is faster and doesn't run out of stack space like the other flood fill function did.

The one downside is that to draw polygons onto other images, this secondary image must be created separately and then the two combined somehow. Usually by my XOR method.

As usual, the pointer to the pixels and the width and height of the image are passed as arguments.
 */
 void scan_fill(uint32_t *pointer,uint32_t width,uint32_t height)
 {
  uint32_t x,y,color0,color1,x1;

  x=0;y=0;
  color0=pointer[x+y*width];
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
testing function for the polygon function
*/
void bbm_polygon_test()
{
 uint32_t *p=NULL,*p1=NULL; /*The pointers to the pixels of each image*/
 uint32_t width=512,height=512; /*The size of the images.*/
 uint32_t x,y; /*variables that may be required for my XOR trick*/
 uint32_t square_size=32; /*size of each square on the checkerboard*/

 int fullturn=360;
 int frame=0,framemax=1;
 int repeat=1;
 char filename[256];

 int points=3;
 double radians=0;

 p=BBM_malloc(width,height);
 p1=BBM_malloc(width,height);

 chastity_checker(p1,width,height,square_size);

 /*framemax=fullturn/points;*/


 while(frame<framemax*repeat)
 {
  sprintf(filename,"./o/%08d.bmp",frame);
  printf("%s\n",filename);

  /*draw the polygon to the first image in three steps*/
  BBM_Fill(p,width,height,0x000000);
  bbm_polygon(p,width,height,width/2,height/2,height*7/16,radians,points,0x00FF80);
  scan_fill(p,width,height);

  /*xor the checkerboard image with the polygon image.*/
  y=0;
  while(y<height)
  {
   x=0;
   while(x<width)
   {
    p[x+y*width]=p[x+y*width]^p1[x+y*width];
    x++;
   }
   y++;
  }

  radians+=2*M_PI/fullturn;
  BBM_Save_BMP(p,width,height,filename,6);
  frame++;
 }
 BBM_free(p);
 BBM_free(p1);
}

/*
a new star polygon function that draws one triangle at a time as part of a star polygon and then uses the scan fill function on it. Because it needs to combine the triangles into the full shape, a temporary pointer to an array of pixels to an image of identical width and height must be used. It's also embarrassingly slow because of so many steps. However, the speed of the frames when turned into a video or gif is the same no matter how slow my program runs.

p is the pointer that will receive the polygon.
ptemp is the temporary pointer which only has one triangle at a time filled.
*/
void bbm_star_polygon(uint32_t *p,uint32_t *ptemp,int width,int height,double cx,double cy,double radius,double radians,int points,int step,uint32_t color)
{
 int i,i1,x,y;

 poly_points(cx,cy,radius,radians,points);
 i=0;
 while(i<points)
 {
  BBM_Fill(ptemp,width,height,0xFF000000);
  i1=(i+step)%npoints;
  bbm_line(ptemp,width,height,color,xpoints[i],ypoints[i],xpoints[i1],ypoints[i1]);
  bbm_line(ptemp,width,height,color,xpoints[i],ypoints[i],cx,cy);
  bbm_line(ptemp,width,height,color,xpoints[i1],ypoints[i1],cx,cy);
  scan_fill(ptemp,width,height);

  /*next the section that bitwise ORs the temporary image with the triangle into the main image which will receive it*/
  y=0;
  while(y<height)
  {
   x=0;
   while(x<width)
   {
    if(ptemp[x+y*width]==color)
    {
     p[x+y*width]=ptemp[x+y*width];
    }
    /*p[x+y*width]=p[x+y*width] | ptemp[x+y*width];*/
    x++;
   }
   y++;
  }

  i+=1;
 }
}


/*Does nothing except display the numbers the points are.*/
void debug_points()
{
 int i=0;
 printf("Debug Points\n");
 while(i<npoints)
 {
  printf("x=%d,y=%d\n",xpoints[i],ypoints[i]);
  i++;
 }
}


/*
Basic function to create an image file containing a regular polygon. All the variables passed have specific meanings that describe some attributes of either the containing image or the polygon itself. This requires the magick command line tool from imagemagick.
*/
void polygon(const char* filename,int width,int height,double cx,double cy,double radius,double radians,int points,uint32_t color)
{
 int x;
 char command[1000],*s;
 poly_points(cx,cy,radius,radians,points);
 s=command;
 s+=sprintf(s,"magick convert -size %dx%d canvas:#000000",width,height);
 s+=sprintf(s," -fill \"#%06X\"",color);
 s+=sprintf(s," -draw \"polygon");
 x=0;
 while(x<npoints)
 {
  s+=sprintf(s," %d,%d",xpoints[x],ypoints[x]);
  x++;
 }
 s+=sprintf(s,"\" %s",filename);
 printf("%s\n",command);
 system(command); /*actually run the command in the shell, same as if typed on the command line*/
}




/*
This function tests the polygon function above.
*/
void polygon_test()
{
 uint32_t *p=NULL,*p1=NULL,*p2=NULL; /*The pointers to the pixels of each image*/
 uint32_t width=512,height=512; /*The size of the images.*/
 int square_size=32; /*size of each square on the checkerboard*/

 int frame=0,framemax=1;

 int points=5;
 double radians=0;
 char filename[256];

 p=BBM_malloc(width,height);

 chastity_checker(p,width,height,square_size);


 while(frame<framemax)
 {

  sprintf(filename,"./o/%08d.bmp",frame);
  printf("%s\n",filename);
 
 polygon(filename,width,height,width/2,height/2,height*7/16,radians,points,0xFFFFFF);
 BBM_Load(&p1,&width,&height,filename);
 polygon(filename,width,height,width/2,height/2,height*3/16,radians,points,0xFFFFFF);
 BBM_Load(&p2,&width,&height,filename);


  if(1) /*if this is nonzero, then XOR pixels from images to make final result.*/
  {
   uint32_t x,y;
   y=0;
   while(y<height)
   {
    x=0;
    while(x<width)
    {
     p2[x+y*width]=p[x+y*width]^p1[x+y*width]^p2[x+y*width];
     x++;
    }
    y++;
   }
  }


  BBM_Save_BMP(p2,width,height,filename,24);

  frame++;
 }

 BBM_free(p);
 BBM_free(p1);
 BBM_free(p2);
}



/*
New method of drawing star polygons using triangles. This draws a series of triangles using the proper corners to make a filled in star polygon no matter the number of points or steps. It constructs a huge imagemagick command drawing many triangles and writes it to a file. Other parts of my BBM library can load the image into pixel array. This is an indirect method of using command line tools to achieve what I could not figure out how to do otherwise. There are no limits anymore.
*/
void star(const char* filename,int width,int height,double cx,double cy,double radius,double radians,int points,int step,uint32_t color)
{
 int x1=0,x2;
 char command[1000],*s;
 poly_points(cx,cy,radius,radians,points);
 s=command;
 s+=sprintf(s,"magick convert -size %dx%d canvas:#000000",width,height);
 s+=sprintf(s," -fill \"#%06X\"",color);
 while(x1<npoints)
 {
  x2=(x1+step)%points;
  s+=sprintf(s," -draw \"polygon ");
  s+=sprintf(s,"%d,%d ",(int)cx,(int)cy);
  s+=sprintf(s,"%d,%d ",xpoints[x1],ypoints[x1]);
  s+=sprintf(s,"%d,%d\"",xpoints[x2],ypoints[x2]);
  x1++;
 }

 s+=sprintf(s," %s",filename);
 printf("%s\n",command);
 system(command);
}


/*
This function tests the star polygon function above.
*/
void star_test()
{
 uint32_t *p=NULL,*p1=NULL,*p2=NULL; /*The pointers to the pixels of each image*/
 uint32_t width=512,height=512; /*The size of the images.*/
 int square_size=32; /*size of each square on the checkerboard*/

 int points=16;
 int step=5;
 double radians=0;
 int frame=0,framemax=360/points; 


 char filename[256];

 p=BBM_malloc(width,height);

 chastity_checker(p,width,height,square_size);

 system("rm ./o/*.bmp"); /*delete previous frames*/

 /*framemax=1;*/   framemax=1;
 while(frame<framemax)
 {

  sprintf(filename,"./o/%08d.bmp",frame);
  printf("%s\n",filename);
 
  star(filename,width,height,width/2,height/2,height*7/16,radians,points,step,0xFFFFFF);
  BBM_Load(&p1,&width,&height,filename);
  star(filename,width,height,width/2,height/2,height*2/16,-radians,points,step,0xFFFFFF);
  BBM_Load(&p2,&width,&height,filename);

  radians+=M_PI/180;

  if(1) /*if this is nonzero, then XOR pixels from images to make final result.*/
  {
   uint32_t x,y;
   y=0;
   while(y<height)
   {
    x=0;
    while(x<width)
    {
     p2[x+y*width]=p[x+y*width]^p1[x+y*width]^p2[x+y*width];
     x++;
    }
    y++;
   }
  }



  BBM_Save_BMP(p2,width,height,filename,24);

  frame++;
 }

 BBM_free(p);
 BBM_free(p1);
 BBM_free(p2);
}




/*
A unique function that is almost a copy of the line drawing algorithm, but instead calls that other function to draw a line between a given center or origin defined by integers cx and cy. The idea behind this was a workaround for filling polygons. However it's unbearably slow and I will need to find a better way.
*/
void line_from_center(uint32_t* p,int width,int height,uint32_t color,int x0,int y0,int x1,int y1,int cx,int cy)
{
 int dx,dy,sx,sy,err,e2;
 dx= x1-x0; if(dx<0){dx=-dx;}
 dy= y1-y0; if(dy<0){dy=-dy;}
 if(x0<x1){sx=1;}else{sx=-1;}
 if(y0<y1){sy=1;}else{sy=-1;}
 err=(dx>dy?dx:-dy)/2;
 while(1)
 {
  /*p[x0+y0*width]=color;*/
  bbm_line(p,width,height,color,cx,cy,x0,y0);
  if (x0==x1 && y0==y1) break;
  e2 = err;
  if (e2 >-dx) { err -= dy; x0 += sx; }
  if (e2 < dy) { err += dx; y0 += sy; }
 }
}


/*
This function draws a regular polygon using the line_from_center function above. This is slow but it was my first attempt at filling a regular polygon
*/
void bbm_polygon_holes(uint32_t* p,int width,int height,double cx,double cy,double radius,double radians,int points,uint32_t color)
{
 int i,i1;
 poly_points(cx,cy,radius,radians,points);
 i=0;
 while(i<npoints)
 {
  i1=(i+1)%npoints;
  line_from_center(p,width,height,color,xpoints[i],ypoints[i],xpoints[i1],ypoints[i1],cx,cy);
  i+=1;
 }
}

