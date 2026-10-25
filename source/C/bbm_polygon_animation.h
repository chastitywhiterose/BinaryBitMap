/*
 This header file was created to help me keep track of my new ideas for polygon animations without modifying the other header files
 which contain the bulk of my code.
*/

/*
A very complex animation involving a spinning polygon.
This one is special because it makes use of 3 separate images. They all use the exact same width and height variables because they are the same size. However they have different pointers for separate pixel data.

p is the image that will be output/displayed each frame
p1 is the image that is filled with a checkerboard before the animation loop begins
p2 is the polygon drawing image that is modified more than anything else.
*/
void bbm_polygon_animation0()
{
 uint32_t *p=NULL,*p1=NULL,*p2=NULL; /*The pointers to the pixels of each image*/
 uint32_t width=32,height=32; /*The size of the images.*/
 uint32_t x,y; /*variables that may be required for my XOR trick*/
 uint32_t square_size=32; /*size of each square on the checkerboard*/

 int fullturn=360;
 int frame=0,framemax=1;
 int repeat=1;
 char filename[256];

 /*variables defining the polygon*/
 int points=6;
 double radius=256;
 double radius_step=4;
 double radians=0;
 uint32_t color=0xFFFFFF;

 p=BBM_malloc(width,height);
 p1=BBM_malloc(width,height);
 p2=BBM_malloc(width,height);

 chastity_checker(p1,width,height,square_size);

 /*framemax=fullturn/points;*/


 BBM_Fill(p,width,height,0x000000);
 while(frame<framemax*repeat)
 {
  sprintf(filename,"./o/%08d.bmp",frame);
  printf("%s\n",filename);

  /*clear the output image with black*/
  BBM_Fill(p,width,height,0x000000);

  radius=height*7/16;
  radius_step=radius/8;
  while(radius>0)
  {
   /*erase the image with black*/
   BBM_Fill(p2,width,height,0x000000);
   /*then draw a polygon with the current radius*/
   bbm_polygon(p2,width,height,width/2,height/2,radius,radians,points,color);
   /*use the scan fill to fill it in*/
   /*scan_fill(p2,width,height);*/


  /*xor the output image with the polygon image.*/


  y=0;
  while(y<height)
  {
   x=0;
   while(x<width)
   {
    p[x+y*width]=p[x+y*width]^p2[x+y*width];
    x++;
   }
   y++;
  }


   /*color^=0x0000FF;*/
   radius-=radius_step;
   radius=0;
  }

  /*xor the output image with the checkerboard image.*/
/*
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
*/

  radians+=2*M_PI/fullturn;
  BBM_Save_BMP(p,width,height,filename,6);
  frame++;
 }
 BBM_free(p);
 BBM_free(p1);
 BBM_free(p2);
}




/*
This is meant to be a smaller and cleaner copy of the first animation in this file.
There are fewer variables and a lot less is going on, but 1 spinning polygon is still happening.
However it has a new feature involving scaling a small image into a big one by calling another function I wrote.
*/

void bbm_polygon_animation1()
{
 uint32_t *pointer=NULL,*pointer1=NULL; /*The pointers to the pixels of each image*/
 uint32_t width=64,height=64; /*The size of the images.*/
 
 uint32_t widthscale=16,heightscale=16; 

 int fullturn=360;
 int frame=0,framemax=1;
 int repeat=1;
 char filename[256];

 /*variables defining the polygon*/
 int points=4;
 double radius=256;

 double radians=0;
 uint32_t color=0xFFFFFF;

 pointer=BBM_malloc(width,height);
 pointer1=BBM_malloc(width*widthscale,height*heightscale);

 /*framemax=fullturn/points;*/
framemax=fullturn/points;

 while(frame<framemax*repeat)
 {
  sprintf(filename,"./o/%08d.bmp",frame);
  printf("%s\n",filename);
  BBM_Fill(pointer,width,height,0x000000);

  radius=height*7/16;

  while(radius>0)
  {
   /*erase the image with black*/
   BBM_Fill(pointer,width,height,0x000000);
   /*then draw a polygon with the current radius*/
   bbm_polygon(pointer,width,height,width/2,height/2,radius,radians,points,color);
   /*use the scan fill to fill it in*/
   /*scan_fill(p2,width,height);*/
   radius=0;
  }
  radians+=2*M_PI/fullturn;

  /*call function to scale image and copy to pointer1*/
  bbm_scale(pointer,width,height,pointer1,widthscale,heightscale);

  /*BBM_Save_BMP(pointer,width,height,filename,1);*/
  BBM_Save_BMP(pointer1,width*widthscale,height*heightscale,filename,1);
  frame++;
 }
 BBM_free(pointer);
 BBM_free(pointer1);
}



/*
animation more designed toward star polygons but may do regular.

The same width and height are used for all the images. The images/pointers all have a specific purpose in this animation.

p is the image that will be output/displayed each frame
p1 is the image that is filled with a checkerboard before the animation loop begins
p2 is the polygon drawing image that is modified more than anything else.
p3 is the temporary image that each triangle is drawn to before being bitwise ORed to the p2 image.
*/

void bbm_star_polygon_animation0()
{
 uint32_t *p=NULL,*p1=NULL,*p2=NULL,*p3=NULL; /*The pointers to the pixels of each image*/
 uint32_t width=512,height=512; /*The size of the images.*/
 uint32_t x,y; /*variables that may be required for my XOR trick*/
 uint32_t square_size=32; /*size of each square on the checkerboard*/

 /*
  variables defining the animation such as the number of frames, the filename of the current frame, and the framerate
  which may apply if it is encoded into a video later.
 */
 int fullturn=360;
 int frame=0,framemax=1,framerate=24;
 int repeat=1;
 char filename[256];
 /*char filename1[256];*/

 /*variables defining the polygon*/
 int points=3;
 int step=1;
 double radius;
 double radius_step;
 double radians=0;
 uint32_t color=0xFFFFFF;

 p=BBM_malloc(width,height);
 p1=BBM_malloc(width,height);
 p2=BBM_malloc(width,height);
 p3=BBM_malloc(width,height);

 /*chastity_checker(p1,width,height,square_size);*/
 chastity_square_tunnel(p1,width,height,square_size);

 /*chastity_gingham(p1,width,height,square_size);*/

 /*framemax=fullturn/points;*/ 

 /*repeat=framerate*60*60;*/

/*points=5;
step=2;
framemax=fullturn/points;
*/

 while(frame<framemax*repeat)
 {
  /*clear the output image with black*/
  BBM_Fill(p,width,height,0x000000);

  radius=height*7/16;
  radius_step=radius;
  while(radius>0)
  {
   /*erase the image with black*/
   BBM_Fill(p2,width,height,0x000000);
   bbm_star_polygon(p2,p3,width,height,width/2,height/2,radius,radians,points,step,color);


  /*xor the output image with the polygon image.*/
  y=0;
  while(y<height)
  {
   x=0;
   while(x<width)
   {
    p[x+y*width]=p[x+y*width]^p2[x+y*width];
    x++;
   }
   y++;
  }

 /*sometimes I like make the radius 0 to make only one polygon be drawn and the loop end*/
    radius-=radius_step;

  }

  /*xor the output image with the checkerboard image.*/
  y=0;
  while(y<height)
  {
   x=0;
   while(x<width)
   {
    p[x+y*width]=p[x+y*width] ^ p1[x+y*width];
    x++;
   }
   y++;
  }

  radians+=2*M_PI/fullturn;

  sprintf(filename,"./o/%08d.bmp",frame);
  printf("%s\n",filename);
  BBM_Save_BMP(p,width,height,filename,1);

  /*sprintf(filename1,"./o/%08d.png",frame);
  BBM_convert_PNG(filename,filename1);*/

  frame++;

  if(frame%fullturn==0)
  {
   step+=1;
   if(step>=(points/2)+points%2)
   {
    points+=1;
    step=1;
   }
  }

 }

 printf("The bitmap frames are created. To make a video file use this command.\n");
 printf("ffmpeg -r %d -f image2 -i o/%%08d.bmp -vcodec libx264 -preset veryslow -crf 23 -pix_fmt yuv420p o/video.mp4",framerate);


 BBM_free(p);
 BBM_free(p1);
 BBM_free(p2);
 BBM_free(p3);
}

/*
a modification of my animation which has a side by side view of the image similar to what I did in allegro.
This one also can draw text to the screen using my previously written bitmap font code in bbm_font.h .
*/
void bbm_star_polygon_animation1()
{
 /*variables for the final output image which will be sent to a screen or image file.*/
 uint32_t *screen=NULL,screen_width=1280,screen_height=720;

 uint32_t *p=NULL,*p1=NULL,*p2=NULL,*p3=NULL; /*The pointers to the pixels of each image*/
 uint32_t width=screen_width/2,height=screen_width/2; /*The size of the smaller images.*/
 uint32_t x,y; /*variables that may be required for my XOR trick*/
 uint32_t square_size=32; /*size of each square on the checkerboard*/

 /*
  variables defining the animation such as the number of frames, the filename of the current frame, and the framerate
  which may apply if it is encoded into a video later.
 */
 int fullturn=360;
 int frame=0,framemax=1,framerate=30;
 int repeat=1;
 char filename[256];

 /*variables defining the polygon*/
 int points=3;
 int step=1;
 double radius;
 double radius_step;
 double radians=0;
 uint32_t color=0xFFFFFF;

 /*temporary string where text is written to.*/
 char s[256];

 screen=BBM_malloc(screen_width,screen_height);

 p=BBM_malloc(width,height);
 p1=BBM_malloc(width,height);
 p2=BBM_malloc(width,height);
 p3=BBM_malloc(width,height);

 bbm_load_font(32,32,"./font/font_32x32.bmp");
 font_line_spacing=4;

 chastity_checker(p1,width,height,square_size);
 /*chastity_stripes_vertical(p1,width,height,square_size);*/

 /*framemax=fullturn/points;*/

 /*repeat=framerate*60*5;*/


 while(frame<framemax*repeat)
 {
  sprintf(filename,"./o/%08d.bmp",frame);
  printf("%s\n",filename);

  /*clear the output image with black*/
  BBM_Fill(p,width,height,0x000000);

  radius=height*7/16;
  radius_step=radius;
  while(radius>0)
  {
   /*erase the image with black*/
   BBM_Fill(p2,width,height,0x000000);
   bbm_star_polygon(p2,p3,width,height,width/2,height/2,radius,radians,points,step,color);


  /*xor the output image with the polygon image.*/
  y=0;
  while(y<height)
  {
   x=0;
   while(x<width)
   {
    p[x+y*width]=p[x+y*width]^p2[x+y*width];
    x++;
   }
   y++;
  }

 /*sometimes I like make the radius 0 to make only one polygon be drawn and the loop end*/
    radius-=radius_step;

  }

  /*xor the output image with the checkerboard image.*/
  y=0;
  while(y<height)
  {
   x=0;
   while(x<width)
   {
    p[x+y*width]=p[x+y*width] ^ p1[x+y*width];
    x++;
   }
   y++;
  }

  radians+=2*M_PI/fullturn;

  BBM_Fill(screen,screen_width,screen_height,0x000000);

  BBM_copy(screen,screen_width,screen_height,p,width,height,0,0);
  BBM_copy(screen,screen_width,screen_height,p2,width,height,screen_width/2,0);

  sprintf(s,"Chastity White Rose. The pure princess\nof pixels,polygons,and ponies!");
  bbm_font_puts(screen,screen_width,screen_height,s,32,height+8);

  sprintf(s,"%d",points);
  bbm_font_puts(screen,screen_width,screen_height,s,screen_width/2+32,32);
  sprintf(s,"%d",step);
  bbm_font_puts(screen,screen_width,screen_height,s,screen_width-64,32);

  sprintf(s,"%d",frame%fullturn);
  bbm_font_puts(screen,screen_width,screen_height,s,screen_width/2+32,height-64);

  BBM_Save_BMP(screen,screen_width,screen_height,filename,1);


  frame++;
  if(frame%fullturn==0)
  {
   step+=1;
   if(step>=(points/2)+points%2)
   {
    points+=1;
    step=1;
   }
  }

 }

 printf("The bitmap frames are created. To make a video file use this command.\n");
 printf("ffmpeg -r %d -f image2 -i o/%%08d.bmp -vcodec libx264 -preset veryslow -crf 23 -pix_fmt yuv420p o/video.mp4",framerate);


 BBM_free(p);
 BBM_free(p1);
 BBM_free(p2);
 BBM_free(p3);
 BBM_free(screen);
 BBM_free(font_pointer);
}

/*
A copy of bbm_star_polygon_animation0 that I made because I wanted to do some heavy editing for a new type of animation without destroying the original. The original was usually used as a solid shape instead of a target of them. The radius_step variable can be set to anything I like but sometimes the program crashes when low numbers are used and so I experiment with it a lot.
*/
void bbm_star_polygon_animation2()
{
 uint32_t *p=NULL,*p1=NULL,*p2=NULL,*p3=NULL; /*The pointers to the pixels of each image*/
 uint32_t width=512,height=512; /*The size of the images.*/
 uint32_t x,y; /*variables that may be required for my XOR trick*/
 uint32_t square_size=32; /*size of each square on the checkerboard*/

 /*
  variables defining the animation such as the number of frames, the filename of the current frame, and the framerate
  which may apply if it is encoded into a video later.
 */
 int fullturn=360;
 int frame=0,framemax=1,framerate=24;
 char filename[256];
 /*char filename1[256];*/

 /*variables defining the polygon*/
 int points=3;
 int step=1;
 double radius;
 double radius_step;
 double radians=0;
 uint32_t color=0xFF00FF;

 int index=0,index1;

/*good youtube resolution*/
width=1280,height=720;


 p=BBM_malloc(width,height);
 p1=BBM_malloc(width,height);
 p2=BBM_malloc(width,height);
 p3=BBM_malloc(width,height);

 chastity_checker(p1,width,height,square_size);
 /*chastity_square_tunnel(p1,width,height,square_size);*/

 /*chastity_gingham(p1,width,height,square_size);*/

 /*framemax=fullturn/points;*/ 

 /*framemax=framerate*60*60;*/

u32bw[0]=0x000000;
u32bw[1]=0xFFFFFF;

/*points=16;
step=7;*/
/*framemax=fullturn/points;*/
/*framemax=framerate*60*5;*/

 while(frame<framemax)
 {
  /*fill with background color*/
  BBM_Fill(p,width,height,0xFF00FF);

  index1=index;

  radius=height*7/16;
  radius_step= 4 /*radius/8*/;
  while(radius>0)
  {
   /*fill with background color*/
   BBM_Fill(p2,width,height,0xFF000000);
   color=u32bw[index];
   bbm_star_polygon(p2,p3,width,height,width/2,height/2,radius,radians,points,step,color);
   index^=1;


  /*xor the output image with the polygon image.*/
  y=0;
  while(y<height)
  {
   x=0;
   while(x<width)
   {
    if(p2[x+y*width]==color)
    {
     p[x+y*width]=p2[x+y*width];
    }
    /*p[x+y*width]=p[x+y*width]^p2[x+y*width];*/
    x++;
   }
   y++;
  }

    radius-=radius_step;

  }
  index=index1;

  /*xor the output image with the checkerboard image.*/
/*
  y=0;
  while(y<height)
  {
   x=0;
   while(x<width)
   {
    p[x+y*width]=p[x+y*width] ^ p1[x+y*width];
    x++;
   }
   y++;
  }
*/

  radians+=2*M_PI/fullturn;

  sprintf(filename,"./o/%08d.bmp",frame);
  printf("%s\n",filename);
  BBM_Save_BMP(p,width,height,filename,6);

  /*optionally flip the index each frame*/
  /*index^=1;*/

  /*sprintf(filename1,"./o/%08d.png",frame);
  BBM_convert_PNG(filename,filename1);*/

  frame++;

  if(frame%fullturn==0)
  {
   step+=1;
   if(step>=(points/2)+points%2)
   {
    points+=1;
    step=1;
   }
  }

 }

 printf("The bitmap frames are created. To make a video file use this command.\n");
 printf("ffmpeg -r %d -f image2 -i o/%%08d.bmp -vcodec libx264 -preset veryslow -crf 23 -pix_fmt yuv420p o/video.mp4",framerate);


 BBM_free(p);
 BBM_free(p1);
 BBM_free(p2);
 BBM_free(p3);
}


