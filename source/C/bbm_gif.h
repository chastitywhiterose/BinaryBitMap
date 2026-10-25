/*
This file will be dedicated to everything about making animated gif files.

For example the first 4 functions are my pixel rolling routines that make absolutely no sense for just 1 frame. I copied them from my SDL code to work with the rest of my BBM library but they shouldn't be included unless the goal is animation.

ImageMagick or any other tool can be used to turn the frames into a GIF file.
*/

/*roll surface pixels to the left*/
void BBM_roll_left(uint32_t *p,int width,int height)
{
 int i=0,x,y=0;
 uint32_t t;
 while(y<height)
 {
  t=p[i];
  x=0;
  while(x<width-1)
  {
   p[i]=p[i+1]; i++;
   x++;
  }
  p[i]=t;i++;
  y++;
 }
}


/*roll surface pixels to the right*/
void BBM_roll_right(uint32_t *p,int width,int height)
{
 int i=width*height-1,x,y=0;
 uint32_t t;
 while(y<height)
 {
  t=p[i];
  x=0;
  while(x<width-1)
  {
   p[i]=p[i-1]; i--;
   x++;
  }
  p[i]=t;i--;
  y++;
 }
}


/*roll surface pixels up*/
void BBM_roll_up(uint32_t *p,int width,int height)
{
 int i=0,x=0,y=0;
 uint32_t t;
 while(x<width)
 {
  i=x;
  t=p[i];
  y=0;
  while(y<height-1)
  {
   p[i]=p[i+width]; i+=width;
   y++;
  }
  p[i]=t;
  x++;
 }
}

/*roll surface pixels down*/
void BBM_roll_down(uint32_t *p,int width,int height)
{
 int i=0,x=0,y=0;
 uint32_t t;
 while(x<width)
 {
  i=width*height-1-x;
  t=p[i];
  y=0;
  while(y<height-1)
  {
   p[i]=p[i-width]; i-=width;
   y++;
  }
  p[i]=t;
  x++;
 }
}


/*
This animation does not roll the checkerboard like most of the others in this file. Instead this is an animation of simply creating different sizes of a checkerboard.
*/
void anim0()
{
 uint32_t *p=NULL; /*The pointer to the pixels*/
 int width=512,height=512; /*The size of the window.*/
 int square_size=1;
 char filename[256];
 int frame=0,framemax=1; framemax=width/2;
 p=BBM_malloc(width,height);
 /*chastity_checker(p,width,height,square_size);*/
 /*chastity_gingham(p,width,height,square_size);*/

 while(frame<framemax)
 {
  sprintf(filename,"o/%08d.bmp",frame);
  printf("%s\n",filename);
  chastity_checker(p,width,height,square_size);

  BBM_Save_BMP(p,width,height,filename,1);

  square_size+=1;

  frame++;
 }
 BBM_free(p);

 if(0)
 {
  system("magick convert -delay 5 -loop 0 o/*.bmp o/0.gif");
  system("rm o/*.bmp");
 }

}


/*
One of my oldest basic animations with a rolling checkerboard. The number of maximum frames will usually be 2 times the size of the squares. This is because animated GIFs infinitely loop and this makes it look the same forever. This only works with checkerboards which was my own discovery by accident.
*/
void anim1()
{
 uint32_t *p=NULL; /*The pointer to the pixels*/
 int width=512,height=512; /*The size of the window.*/
 int square_size=16;
 char filename[256];
 int frame=0,framemax=square_size*2; /*framemax=1;*/
 p=BBM_malloc(width,height);
 chastity_checker(p,width,height,square_size);
 /*chastity_checker_parallelogram(p,width,height,square_size);*/
 /*chastity_gingham(p,width,height,square_size);*/

u32bw[0]=0x000000;
u32bw[1]=u32bw[0]^0xFFFFFF;

 /*chastity_square_tunnel(p,width,height,square_size);*/

 while(frame<framemax)
 {
  sprintf(filename,"o/%08d.bmp",frame);
  printf("%s\n",filename);
  BBM_Save_BMP(p,width,height,filename,24);

  /*BBM_roll_left(p,width,height);*/
  /*BBM_roll_right(p,width,height);*/
  BBM_roll_left(p,width,height);
  /*BBM_roll_up(p,width,height);*/
  frame++;
 }
 BBM_free(p);

 if(0)
 {
  system("magick convert -delay 5 -loop 0 o/*.bmp o/0.gif");
  system("rm o/*.bmp");
 }

}


 



/*
This next animation is much more tricky. It involves loading an image and then making a second image based on it.
*/
void anim2()
{
 uint32_t *p=NULL,*p1,*p2; /*The pointers to the pixels*/
 uint32_t width=1280,height=720; /*The size of the window.*/
 int square_size=32;
 char filename[256];

 uint32_t frame=0,framemax=1,x,y;

 /*First load the image from the PPM file*/
 BBM_Load(&p,&width,&height,"./i/logo/BBM_logo.ppm");
 if(p==NULL){printf("Pointer is NULL\n"); return;} /*Must error check in case could not read file.*/
 

 /*Then use the width and height from the last to make a new image of the same size.*/
 p1=BBM_malloc(width,height);
 square_size=width/8;
 chastity_checker(p1,width,height,square_size);

 /*Then make a third image of the same size.*/
 p2=BBM_malloc(width,height);


 framemax=square_size*2;/*framemax=1;*/
 while(frame<framemax)
 {
  sprintf(filename,"o/%08d.bmp",frame);
  /*printf("%s\n",filename);*/

  y=0;
  while(y<height)
  {
   x=0;
   while(x<width)
   {
    p2[x+y*width]=p[x+y*width]^p1[x+y*width];
    x++;
   }
   y++;
  }

  BBM_Save_BMP(p2,width,height,filename,1);
  BBM_roll_left(p1,width,height);

  frame++;
 }

 BBM_free(p);
 BBM_free(p1);
 BBM_free(p2);

 if(0)
 {
  system("magick convert -delay 5 -loop 0 o/*.bmp o/0.gif");
  system("rm o/*.bmp");
 }

}

/*
 This is intended to be used on frames of a video extracted with ffmpeg. As such it's useless unless I set it up right first. The proper ffmpeg command can do it. I have a separate text file I plan to publish which has what I need to know to set it up. The directories where the frames are stored is hard coded here and I have to change the source depending on location.

The purpose of this is to take any video and change the colors to the bit depth I want to see how it looks. My favorite is black and white but I can do many others.
*/
void anim3()
{
 uint32_t *p=NULL,*p1,*p2; /*The pointers to the pixels*/
 uint32_t width=1280,height=720; /*The default size of the image.*/
 char filename[256];
 int square_size=32;

 int frame=1,framemax=44000;

  sprintf(filename,"i/frames/%08d.bmp",frame);
 /*load first frame to get dimensions*/
 /*BBM_Load(&p,&width,&height,filename);*/  /*slow loading function that tries to identify the file by extension.*/
  BBM_Load_BMP(&p,&width,&height,filename);  /*faster loading by assuming Windows BMP file.*/
  if(p==NULL){printf("Pointer is NULL\n"); return;} /*Must error check in case could not read file.*/

 /*Then use the width and height from the last to make a new image of the same size.*/
 p1=BBM_malloc(width,height);
 chastity_checker(p1,width,height,square_size);

 /*Then make a third image of the same size.*/
 p2=BBM_malloc(width,height);

 while(frame<=framemax)
 {

  sprintf(filename,"i/frames/%08d.bmp",frame);
  BBM_Load_BMP(&p,&width,&height,filename);
  if(p==NULL){printf("Pointer is NULL\n"); break;}

  if(0) /*if this is nonzero, then XOR pixels from p and p1 together to make p2 image.*/
  {
   uint32_t x,y;
   y=0;
   while(y<height)
   {
    x=0;
    while(x<width)
    {
     p2[x+y*width]=p[x+y*width]^p1[x+y*width];
     x++;
    }
    y++;
   }
  }


  /*BBM_Set_BPP(p,width,height,6);*/

  /*bbm_palette_count(p,width,height);*/

  sprintf(filename,"o/%08d.bmp",frame);
  BBM_Save_BMP(p,width,height,filename,1);


  /*BBM_roll_left(p1,width,height);*/

  frame++;
 }

 printf("frame==%d\n",--frame);

 if(p!=NULL){free(p);}
 if(p1!=NULL){free(p1);}
 if(p2!=NULL){free(p2);}

}

/*
I have a file named "BBM Animation.txt" which has commands I frequently use for turning my frames into the gif animations or video files I have uploaded to Facebook. I'll publish it when it's pretty enough.
*/

/*
 A modified rolling checkerboard. What makes this different is that new rolling functions are used to roll only parts of the image.
*/

void anim4()
{
 uint32_t *p=NULL; /*The pointer to the pixels*/
 uint32_t width=512,height=512; /*The size of the window.*/
 int square_size=16;
 char filename[256];

 int frame=0,framemax=square_size*2;/*framemax=1;*/

 p=BBM_malloc(width,height);

 chastity_checker(p,width,height,square_size);
 /*chastity_gingham(p,width,height,square_size);*/

 while(frame<framemax*1)
 {
  /*Uncomment the following section to make Windows BMP files as frames.*/

  sprintf(filename,"o/%08d.bmp",frame);
  printf("%s\n",filename);
  BBM_Save_BMP(p,width,height,filename,1);


  BBM_roll_part_down(p,width,height,0,0,width/2,height/2);
  BBM_roll_part_right(p,width,height,0,width/2,width/2,height/2);
  BBM_roll_part_up(p,width,height,width/2,0,width/2,height/2);
  BBM_roll_part_left(p,width,height,width/2,height/2,width/2,height/2);

  frame++;
 }

 BBM_free(p);

 if(0)
 {
  system("magick convert -delay 5 -loop 0 o/*.bmp o/0.gif");
  system("rm o/*.bmp");
 }

}

/*
 Another mini rolling checkerboard animation.
*/

void anim5()
{
uint32_t *p=NULL; /*The pointer to the pixels*/
 int width=512,height=512; /*The size of the window.*/
 int square_size=16;
 char filename[256];

 int frame=0,framemax=square_size*2;/*framemax=1;*/

 p=BBM_malloc(width,height);

 chastity_checker(p,width,height,square_size);
 /*chastity_gingham(p,width,height,square_size);*/

 while(frame<framemax*1)
 {


  sprintf(filename,"o/%08d.bmp",frame);
  printf("%s\n",filename);
  BBM_Save_BMP(p,width,height,filename,1);


  BBM_roll_part_down(p,width,height,0,0,width/2,height/2);
  BBM_roll_part_right(p,width,height,0,width/2,width/2,height/2);
  BBM_roll_part_down(p,width,height,width/2,0,width/2,height/2);
  BBM_roll_part_left(p,width,height,width/2,height/2,width/2,height/2);

  BBM_roll_part_right(p,width,height,0,0,width/2,height/2);
  BBM_roll_part_up(p,width,height,0,width/2,width/2,height/2);
  BBM_roll_part_left(p,width,height,width/2,0,width/2,height/2);
  BBM_roll_part_up(p,width,height,width/2,height/2,width/2,height/2);

  frame++;
 }

 BBM_free(p);

 if(0)
 {
  system("magick convert -delay 5 -loop 0 o/*.bmp o/0.gif");
  system("rm o/*.bmp");
 }

}



/*
By combining my own font library with my checkerboard style animations, I decided to use text to say what I feel like saying at the time but catch people's attention with the checkerboard. All polygon drawing code has been removed from this one because it was unnecessary because polygons other than the squares of a checkerboard are not the focus.

This must be coded very carefully. The framerate is the same as the size of the squares for good reason.
*/
void bbm_checker_multi_roll()
{
 uint32_t *p=NULL,*p1=NULL,*p2=NULL; /*The pointers to the pixels of each image*/
 uint32_t width=640,height=640; /*The size of the smaller images.*/
 uint32_t x,y; /*variables that may be required for my XOR trick*/
 uint32_t square_size=64; /*size of each square on the checkerboard*/

 /*
  variables defining the animation such as the number of frames, the filename of the current frame, and the framerate
  which may apply if it is encoded into a video later.
 */

 int frame=0,framerate=square_size;
 int seconds=0,minutes=0;
 char filename[256];

 /*temporary string where text is written to.*/
 char s[256];

 p=BBM_malloc(width,height);
 p1=BBM_malloc(width,height);
 p2=BBM_malloc(width,height);


 bbm_load_font(64,64,"./font/font_64x64.bmp");
 font_line_spacing=64;

 chastity_checker(p1,width,height,square_size);
 /*chastity_stripes_vertical(p1,width,height,square_size);*/

 /*setup the image with the text*/
 BBM_Fill(p2,width,height,0x000000);
 sprintf(s,"Checker\nLives\nMatter");
 bbm_font_puts(p2,width,height,s,square_size*2+4,square_size*2+4);

 while(minutes<4)
 {
  sprintf(filename,"./o/%08d.bmp",frame);
  printf("%s\n",filename);

  /*xor the output image with the checkerboard image.*/
  y=0;
  while(y<height)
  {
   x=0;
   while(x<width)
   {
    p[x+y*width]=p1[x+y*width] ^ p2[x+y*width];
    x++;
   }
   y++;
  }

  if(minutes==0){BBM_roll_full_left(p1,width,height);}
  if(minutes==1){BBM_roll_full_up(p1,width,height);}
  if(minutes==2){BBM_roll_full_right(p1,width,height);}
  if(minutes==3){BBM_roll_full_down(p1,width,height);}

  BBM_Save_BMP(p,width,height,filename,1);

  frame++;
  if(frame%framerate==0)
  {
   seconds++;
   if(seconds%60==0){minutes++;}
  }


 }

 printf("The bitmap frames are created. To make a video file use this command.\n");
 printf("ffmpeg -r %d -f image2 -i o/%%08d.bmp -vcodec libx264 -preset veryslow -crf 23 -pix_fmt yuv420p o/video.mp4",framerate);


 BBM_free(p);
 BBM_free(p1);
 BBM_free(p2);
 BBM_free(font_pointer);
}

/*
The code for some of this was copied from the chastity_square_tunnel function in bbmlib.h but was modified to change from frame to frame. Each time it fills the whole image. The bitcount variable controls when the color index changes between the two colors.
*/
void anim_square_tunnel()
{
 uint32_t *p=NULL; /*The pointer to the pixels*/
 int width=512,height=512; /*The size of the window.*/
 int square_size=16;
 char filename[256];
 int frame=0,framemax=square_size*2; 
 int bitcount1=0,bitcount,index=0; /*controls when the starting color changes*/

 p=BBM_malloc(width,height);


u32bw[0]=0x000000;
u32bw[1]=u32bw[0]^0xFFFFFF;

/*framemax=1;*/ 
 while(frame<framemax)
 {
  /*drawing section*/
  int x,y,x1=0,y1=0,x2=width,y2=height;
  bitcount=bitcount1;
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
   x1+=1;
   y1+=1;
   x2-=1;
   y2-=1;
   bitcount+=1;
   if(bitcount%square_size==0){index^=1;}
  }
  /*drawing section end*/
  bitcount1+=1;
  if(bitcount1%square_size==0){index=1;bitcount1=0;}

  sprintf(filename,"o/%08d.bmp",frame);
  printf("%s\n",filename);
  BBM_Save_BMP(p,width,height,filename,1);
  frame++;


 }
 BBM_free(p);

 if(0)
 {
  system("magick convert -delay 5 -loop 0 o/*.bmp o/0.gif");
  system("rm o/*.bmp");
 }

}

