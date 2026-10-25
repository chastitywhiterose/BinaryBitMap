/*Header file for the color palette used in some bitmaps.*/

#define bbm_palette_max 0x1000000

/*global variables to be used to store the colors and how many there are*/
uint32_t bbm_palette[bbm_palette_max],bbm_palette_length=0;

void bbm_palette_view()
{
 uint32_t x=0;
 while(x<bbm_palette_length)
 {
  printf("bbm_palette[%d]=%06X;\n",x,bbm_palette[x]);
  x++;
 }
 printf("bbm_palette_length=%d;\n",bbm_palette_length);
}


/*
 This function was copied from a very old project known as CK3D. It attempts to make a rainbow using the integer n passed to it using degrees of red,green,blue.
 It seems that the number of colors is equal to 6*n. This means that if n is 20, you get 6*20==120;
 The max value n can be is 255 which gives 1530 colors! However my favorite is n=40 because 240 colors is really convenient for fitting under the 256 color gif limit.
*/
void bbm_palette_rainbow(int n)
{
 int x,color;
 int red=n,green=0,blue=0;
 x=0;
 while(green<n)
 {
  color=255*red/n;color*=256;color+=255*green/n;color*=256;color+=255*blue/n;bbm_palette[x]=color;
  x++;
  green++;
 }
 while(red>0)
 {
  color=255*red/n;color*=256;color+=255*green/n;color*=256;color+=255*blue/n;bbm_palette[x]=color;
  x++;
  red--; 
 }
 while(blue<n)
 {
  color=255*red/n;color*=256;color+=255*green/n;color*=256;color+=255*blue/n;bbm_palette[x]=color;
  x++;
  blue++;
 }
 while(green>0)
 {
  color=255*red/n;color*=256;color+=255*green/n;color*=256;color+=255*blue/n;bbm_palette[x]=color;
  x++;
  green--; 
 }
 while(red<n)
 {
  color=255*red/n;color*=256;color+=255*green/n;color*=256;color+=255*blue/n;bbm_palette[x]=color;
  x++;
  red++; 
 }
 while(blue>0)
 {
  color=255*red/n;color*=256;color+=255*green/n;color*=256;color+=255*blue/n;bbm_palette[x]=color;
  x++;
  blue--; 
 }
 bbm_palette_length=x;
}

/*a function for testing how many colors are created with my rainbow function*/
void bbm_palette_rainbow_test(int n)
{
 bbm_palette_rainbow(n);
 bbm_palette_view();
}

/*
 make an animation based on the rainbow. The squares of a checkerboard change based on the current palette set with the rainbow function. Normally I would have animations in a separate file but some things only work in the context of functions in this file that make palettes.
*/
void bbm_palette_rainbow_anim(int n)
{
 uint32_t *p=NULL; /*The pointer to the pixels*/
 uint32_t width=720,height=720; /*The size of the window.*/
 int square_size=16;
 char filename[256];
 int frame=0,framemax=square_size*2;/*framemax=1;*/
 int palette_index=0;

 bbm_palette_rainbow(n);
 framemax=bbm_palette_length;

 p=BBM_malloc(width,height);

 

 system("rm ./o/*.bmp"); /*delete previous frames*/

 while(frame<framemax)
 {
  u32bw[0]=bbm_palette[palette_index];

  u32bw[1]=u32bw[0]^0xFFFFFF;

  chastity_checker(p,width,height,square_size);
  /*chastity_gingham(p,width,height,square_size);*/
  palette_index++; palette_index%=bbm_palette_length;
  /*printf("index %d\n",palette_index);*/

  sprintf(filename,"o/%08d.bmp",frame);
  printf("%s\n",filename);
  BBM_Save_BMP(p,width,height,filename,24);

  frame++;
 }

 BBM_free(p);

 if(0)
 {
  system("gm convert -delay 5 -loop 0 o/*.bmp o/0.gif");
  system("rm o/*.bmp");
 }

}


/*
 A very weird animation based on copying some code from my checkerboard drawing function but changing the color every row!
*/
void bbm_palette_rainbow_anim2(int n)
{
 uint32_t *p=NULL; /*The pointer to the pixels*/
 uint32_t width=720,height=720; /*The size of the window.*/
 int square_size=16;
 char filename[256];
 int frame=0,framemax=square_size*2;/*framemax=1;*/
 int palette_index=0;
 int palette_index1=0;

 bbm_palette_rainbow(n);
 framemax=bbm_palette_length;

 p=BBM_malloc(width,height);

 

 system("rm ./o/*.bmp"); /*delete previous frames*/
 
 palette_index1=bbm_palette_length/2;

 while(frame<framemax)
 {
  uint32_t x,y=0,index=0,index1,bitcountx,bitcounty=0;


 /*checkerboard drawing starts here*/
 while(y<height)
 {
  
  u32bw[0]=bbm_palette[palette_index];
  /*u32bw[1]=u32bw[0]^0xFFFFFF;*/
  u32bw[1]=bbm_palette[palette_index1];
  
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
  
  palette_index++; palette_index%=bbm_palette_length;
  palette_index1++; palette_index1%=bbm_palette_length;
  
 }
   /*checkerboard drawing ends here*/

  palette_index++; palette_index%=bbm_palette_length;
  palette_index1++; palette_index1%=bbm_palette_length;
  /*printf("index %d\n",palette_index);*/

  sprintf(filename,"o/%08d.bmp",frame);
  printf("%s\n",filename);
  BBM_Save_BMP(p,width,height,filename,24);

  frame++;
 }

 BBM_free(p);

 if(0)
 {
  system("gm convert -delay 5 -loop 0 o/*.bmp o/0.gif");
  system("rm o/*.bmp");
 }

}




/*
A function to set up the global palette for my other needs according to the bpp that I have predefined to be valid. A very bbm specific system I made.
*/
void bbm_palette_make(int bpp)
{
 uint32_t x,gray,bitcount,color,red,green,blue;

 bbm_palette_length=1<<bpp;

 /* if bpp is one of these, make the palette be grayscale. */
 if(bpp==1||bpp==2||bpp==4||bpp==8)
 {
  printf("making %d bpp grayscale palette\n",bpp);
  x=0;
  while(x<bbm_palette_length)
  {
   gray=x;
   /*convert gray into a 24 bit RGB equivalent.*/
   color=0;
   bitcount=0;
   while(bitcount<24)
   {
    color<<=bpp;
    color|=gray;
    bitcount+=bpp;
   }

   bbm_palette[x]=color;

   x++;
  }
 }

 else if(bpp==3||bpp==6||bpp==12||bpp==24)
 {
  printf("making %d bpp full color palette\n",bpp);

  bpp/=3;

  gray=(1<<bpp)-1;

  /*printf("graymask == %d\n",gray);*/

  x=0;
  while(x<bbm_palette_length)
  {
   color=x;
   blue=color&gray;color>>=bpp;
   green=color&gray;color>>=bpp;
   red=color&gray;color>>=bpp;

   /*printf("red=%d,green=%d,blue=%d\n",red,green,blue);*/

   /*next left shift red and green back to their proper position. blue doesn't need to be shifted.*/
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

   bbm_palette[x]=color;

   x++;
  }
 }

 else{printf("bpp of %d not supported.\n",bpp);return;}

}




/*
a function which reads from an existing image and counts how many colors there are
I don't have much use for it now but it's cool that I wrote it. It might be useful later for setting up custom palettes.
*/
void bbm_palette_count(uint32_t *p,int width,int height)
{
 int x=0,y,colors;

 /*pass 1, set entire array to 0s*/
 while(x<bbm_palette_max)
 {
  bbm_palette[x]=0;
  /*printf("bbm_palette[%d]=%06X\n",x,bbm_palette[x]);*/
  x++;
 }

 /*pass 2, go through entire image and increment any color index that appears*/
 x=0,y=0;
 while(y<height)
 {
  x=0;
  while(x<width)
  {
   bbm_palette[ p[x+y*width] ]++;
   x++;
  }
  y++;
 }

 colors=0;
 x-=0;
 while(x<bbm_palette_max)
 {
  if(bbm_palette[x]!=0)
  {
   /*printf("bbm_palette[%06X]=%d\n",x,bbm_palette[x]);*/
   colors++;
  }
  x++;
 }


 printf("number of colors=%d\n",colors);
}

/*
A function to test the palette by making a series of checkerboards with color combinations.
*/

void bbm_palette_test(int bpp)
{

 uint32_t *p=NULL; /*The pointer to the pixels*/
 uint32_t width=512,height=512; /*The size of the window.*/
 int square_size=16;
 char filename[256];
 int frame=0,framemax=square_size*2;/*framemax=1;*/
 int palette_index=0;
 int palette_index1;

 bbm_palette_make(bpp);
 framemax=bbm_palette_length;


 p=BBM_malloc(width,height);




 /*chastity_gingham(p,width,height,square_size);*/

 system("rm ./o/*.bmp"); /*delete previous frames*/

 while(frame<framemax)
 {


  u32bw[0]=bbm_palette[palette_index];

  palette_index1=bbm_palette_length-1-palette_index;

  u32bw[1]=bbm_palette[palette_index1];

  chastity_checker(p,width,height,square_size);

  palette_index++; palette_index%=bbm_palette_length;

 /* printf("index %d\n",palette_index);*/

  sprintf(filename,"o/%08d.bmp",frame);
  printf("%s\n",filename);
  BBM_Save_BMP(p,width,height,filename,24);


  frame++;
 }

 BBM_free(p);

}
