/*
This file has functions about loading a specific bitmap font in a custom format.

I have my own custom font file format which is technically a Windows BMP format exactly as output by imagemagick.
For example.

magick convert fixedsys-8x15.png fixedsys-8x15.bmp

But this only applies after I make specifically cropped screenshots of characters. The width and height of each character must be passed to the bbm_load_font function as arguments. Only I would know what they are because I created the images in the first place. This system is highly specific and at risk of being very buggy if used incorrectly but it's also as flexible as I need. I can create any type of font theoretically if I took enough time.
*/

uint32_t font_char_width,font_char_height; /*width and height of a single character.*/
uint32_t font_image_width,font_image_height; /*width and height of the font image.*/
uint32_t font_chars_per_row;
uint32_t *font_pointer;
uint32_t font_line_spacing=0; /*defines how much extra space between lines of text*/

void bbm_load_font(uint32_t width,uint32_t height,const char *font_filename)
{
 printf("font loading function called with\nwidth=%d height=%d filename=%s\n",width,height,font_filename);
 BBM_Load_BMP(&font_pointer,&font_image_width,&font_image_height,font_filename);

 font_char_width=width;
 font_char_height=height;

 printf("The font is loaded.\n");
 printf("The size of each char is %dx%d pixels.\n",font_char_width,font_char_height);
 printf("The size of loaded font image is %dx%d pixels.\n",font_image_width,font_image_height);
 
 font_chars_per_row=font_image_width/font_char_width;
 printf("The font image has %d characters per row.\n",font_chars_per_row);
 
}

/*
a long overly complex function to output a single character from the bitmap font by direct pixel copying regions of the image.
It only works if I hard code the math behind where each character is in the font image.
*/
void bbm_font_putchar(uint32_t *pointer,uint32_t width,uint32_t height,uint32_t character,uint32_t x,uint32_t y)
{
 uint32_t x1,y1,x2,y2;
/*
 printf("Trying to output character '%c'.\n",character);
 printf("At position %d,%d.\n",x,y);
*/

 /*
  character is expected to be in range of 0x20 to 0x7E
  I subtract 0x20 to make it between 0x00 and 0x5E.
 */
 character-=0x20;
 
 x1=character%font_chars_per_row*font_char_width;
 y1=character/font_chars_per_row*font_char_height;
 x2=x1+font_char_width;
 y2=y1+font_char_height;

 /*printf("This character found in font bitmap at position %d,%d.\n",x1,y1);*/

 while(y1<y2)
 {
  while(x1<x2)
  {
   pointer[x+y*width]=font_pointer[x1+y1*font_image_width];
   x+=1;
   x1+=1;
  }
  x-=font_char_width;
  x1-=font_char_width;

  y+=1;
  y1+=1;
 }
}

/*
 this prints a string to the image. The '\n' character is special and makes it go to the next line. I can also edit the global "font_line_spacing" variable to extra pixels of space between rows of text because sometimes the pixels of the characters touch.
*/
void bbm_font_puts(uint32_t *pointer,uint32_t width,uint32_t height,const char *s,uint32_t x,uint32_t y)
{
 uint32_t x1=x,i=0;

 while(s[i]!=0)
 {
  if(s[i]=='\n')
  {
   x=x1;y+=font_char_height;
   y+=font_line_spacing;
  }
  else
  {
   bbm_font_putchar(pointer,width,height,s[i],x,y);
   x+=font_char_width;
  }
  i+=1;
 }
}

/*
A function to make sure my font system works.
*/
void bbm_font_test()
{
 /*variables for our output image*/
 uint32_t *pointer;
 uint32_t width=512,height=512;

 pointer=BBM_malloc(width,height);

 BBM_Fill(pointer,width,height,0x000000);

 /*first load the font into memory using the global variables at the top of this file.*/
 /*bbm_load_font(8,8,"./font/font_8x8.bmp");*/
 /*bbm_load_font(16,16,"./font/font_16x16.bmp");*/
 /*bbm_load_font(32,32,"./font/font_32x32.bmp");*/
 /*bbm_load_font(64,64,"./font/font_64x64.bmp");*/

 bbm_load_font(8,15,"./font/fixedsys-8x15.bmp");

 /*save the image to a file just so I can see that it is correct.*/
 BBM_Save_BMP(font_pointer,font_image_width,font_image_height,"./o/font.bmp",24);

 /*bbm_font_putchar(pointer,width,height,'4',100,100);*/

 bbm_font_puts(pointer,width,height,"Hello\nWorld!",50,100);

 BBM_Save_BMP(pointer,width,height,"./o/out.bmp",24);

 BBM_free(pointer);
 BBM_free(font_pointer);
}


/*
This is the first of my many animations that are now possible using only text! I have been writing powers of two programs for many years but now it's possible to make a video file of them!

*/

void bbm_font_pow2()
{
 /*variables for our output image*/
 uint32_t *pointer;
 uint32_t width=1280,height=720;

 /*animation related variables*/
 int frame=0,framemax=1,framerate=30;
 char filename[256];
 int x,y,x1; /*index variables used for many purposes*/
 
 /*this next section sets up variables for the powers of two algorithm using an array of chars.*/
 int length=256; /*how many digits the array can use at maximum*/
 int length2=1; /*How many digits are currently used. This will increase over time!*/
 char* a=(char*)malloc(length*sizeof(*a));if(a==NULL){printf("memory allocation failed\n");return;}

 for(x=0;x<length;x++){a[x]=0;} a[0]=1; /*set up the array*/

 pointer=BBM_malloc(width,height); /*allocate pixel memory*/

 /*load the font so it's ready for use*/
  /*bbm_load_font(32,32,"./font/font_32x32.bmp");*/
  bbm_load_font(8,15,"./font/fixedsys-8x15.bmp");

  /*clear the output image with black*/
  BBM_Fill(pointer,width,height,0x000000);

 /*
  this animation depends greatly on the height of the font characters
  for every font_char_height number of frames, one more power of two is displayed.
  this allows for extreme control of how long it goes
 */
 /*framemax=font_char_height*160;*/
 framemax=framerate*60*60;

/*framemax=1;*/

 /*the main loop begins!*/
 while(frame<framemax)
 {
  sprintf(filename,"./o/%08d.bmp",frame);
  printf("%s\n",filename);

  /*the text drawing does not happen every frame but only when enough scrolling has happened*/
  if(frame%font_char_height==0)
  {
   /*optionally, output the array as real text to the console*/
   for(x=length2-1;x>=0;x--){printf("%i",a[x]);} printf("\n");

   /*next set up the initial location of text drawing*/
   x1=0;
   x=width-font_char_width;
   y=height-font_char_height;
   while(x1<length2)
   {
    bbm_font_putchar(pointer,width,height,a[x1]+'0',x,y);
    x-=font_char_width;
    x1++;
   }

   /*routine to double every digit*/
   y=0;
   for(x=0;x<=length2;x++)
   {
    a[x]+=a[x]; /*Add this digit to itself!*/
    a[x]+=y;    /*Add the carry!*/
    if(a[x]>9){y=1;a[x]-=10;}else{y=0;}
   }
   if(a[length2]>0){length2++;}

  }
  
  BBM_Save_BMP(pointer,width,height,filename,1);
  BBM_roll_full_up(pointer,width,height);

  /*fill bottom row of pixels*/
  bbm_rect(pointer,width,height,0,height-1,width,1,0x000000);

  frame++;
 }

 printf("The bitmap frames are created. To make a video file use this command.\n");
 printf("ffmpeg -r %d -f image2 -i o/%%08d.bmp -vcodec libx264 -preset veryslow -crf 23 -pix_fmt yuv420p o/video.mp4",framerate);

 free(a);
 BBM_free(pointer);
 BBM_free(font_pointer);
}

/*
A function to read a text file and try to turn it into an image.
This has to manually word wrap based on the length of "words" .
This basically means strings of visible characters.
Spaces and newlines are handled separately.
*/
void bbm_font_text_file()
{
 /*variables for the text file*/
 FILE *fp;
 char filename[256];
 char *charpointer,*s,*s1;
 int memlength=0x100000,filelength,findex=0,c;

 /*variables for our output image*/
 uint32_t *pointer;
 uint32_t width=720,height=720*8;

 /*variables for location of where text is drawn*/
 uint32_t x=0,y=0,wordlength,wordlengthpixels;

/*try to malloc a large amount of memory to fit in an entire text file.*/
 charpointer=(char*)malloc(memlength*sizeof(*charpointer));
 if(charpointer==NULL){printf("Error: malloc failed,\n");return;}
 
 printf("%d bytes of memory for text string allocated\n",memlength);

 sprintf(filename,"./bbm_font.h");

 fp=fopen(filename,"rb");
 if(fp==NULL){printf("Failed to read file \"%s\".\n",filename); return;}
 else{printf("File \"%s\" opened.\n",filename);}

 filelength=fread(charpointer,1,memlength,fp);
 fclose(fp);

 printf("%d bytes read from file\n",filelength);

 /*next start allocating memory for images and loading fonts.*/
 pointer=BBM_malloc(width,height);

 BBM_Fill(pointer,width,height,0x000000);

 /*first load the font into memory using the global variables at the top of this file.*/
 /*bbm_load_font(8,8,"./font/font_8x8.bmp");*/
 bbm_load_font(8,15,"./font/fixedsys-8x15.bmp");

 /*next begin the text drawing loop!*/
 s=charpointer;s1=s;wordlength=0;

 while(y<height)
 {
  while(*s1<0x21)
  {
   findex++; if(findex==filelength){break;}
   if(*s1==' '){x+=font_char_width;}
   if(*s1=='\n'){x=0;y+=font_char_height;}
   s1++;
  }

  wordlength=0; /*wordlength starts at 0.*/
  s=s1; /*found non whitespace character and set the beginning of the new string*/

  while(*s1>=0x21 && *s1<=0x7E) /*find next whitespace character and set it to zero for valid C string*/
  {
   s1++;
   wordlength++;
   findex++; if(findex==filelength){break;}
  }
  if(findex==filelength){break;}
  wordlengthpixels=wordlength*font_char_width;

  c=*s1; /*save the current whitespace char into variable c.*/
  *s1=0; /*replace it with zero because other C functions expect 0 terminated string.*/
  /*printf("%s\n",s);*/

  if(x+wordlengthpixels>=width){x=0;y+=font_char_height;}  
  if(y>=height){printf("Image not tall enough to print whole file.\n");break;}
  bbm_font_puts(pointer,width,height,s,x,y);
  x+=wordlengthpixels;

 /*
  printf("x=%d\n",x);
  printf("wordlength=%d\n",wordlength);
  printf("wordlengthpixels=%d\n",wordlengthpixels);
 */

  *s1=c; /*now put c back into the char array pointer to by s1 so it will be processed properly at the beginning of the loop.*/
 }

 BBM_Save_BMP(pointer,width,height,"./o/out.bmp",1);

 BBM_free(pointer);
 BBM_free(font_pointer);
 free(charpointer);

}



