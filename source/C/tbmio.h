/*The functions in this file are never actually used for making real image files but could theoretically serve me in some way in the future.*/

/*
A function for outputting text in a neat format so I can later possibly create my own bitmap fonts out of it.
Usually called with something like:

BBM_Save_ASCII("o/ascii.txt");

*/
void BBM_Save_ASCII(const char *filename)
{
 int x,y,i;
 FILE* fp;
 fp=fopen(filename,"wb+");
 if(fp==NULL){printf("Failed to create file \"%s\".\n",filename); return;}
 i=0;
 y=0;
 while(y<16)
 {
  x=0;
  while(x<16)
  {
   if(i<0x20 || i>0x7E){fputc(' ',fp);}
   else {fputc(i,fp);}
   i+=1;
   x+=1;
  }
  fputc('\n',fp);
  y+=1;
 }
 
 fclose(fp);
}

/*
Information on fonts that I may need to reference.

https://en.wikipedia.org/wiki/Code_page_437
*/

/*
 I finally decided to put my very first code I wrote, long before I got into graphics programming, to a good use.
 For years I specialized in outputting the text of common integer sequences, often in binary, my favorite base.
 This is a perfect thing to use when trying to make a text version of my bitmaps.
 */

/*First set the size of the array to be defined next*/
#define unicorn_length 65536
 /*string is unicorn_length + 1 extra character for the terminating zero.*/
char unicorn[unicorn_length+1];

/*
 This function can return a string from an integer using any base from 2 to 36.
 It even accepts a minimum width of digits. It always returns the address of the unicorn array
 and this is never a problem because it's only used temporarily when passed to printf in other functions.
*/

char* inttostr(unsigned long l,int base,int width)
{
 char *s=unicorn+unicorn_length;
 *s=0;
 do
 {
  s--;
  *s=l%base;
  l/=base;
  if(*s<10){*s+='0';}else{*s=*s+'A'-10;}
  width--;
 }
 while(l!=0 || width>0);
 return s;
}


/*
 TBM: Text Bit Map
 Text form of my BBM format. This is used for debugging purposes to see the bits in a text format and diagnose any problems.
 Although theoretically this could be turned into a separate format for saving and loading data, it's just too wasteful in space.
 The point however is to see the results without writing to an actual image format like all my other functions.
*/
void BBM_Save_TBM(uint32_t *p,uint32_t width,uint32_t height,const char* filename,int bpp)
{
 uint32_t x,y,pixel,r,g,b,gray,bits;

 FILE* fp;
 fp=fopen(filename,"wb+");
 if(fp==NULL){printf("Failed to create file \"%s\".\n",filename); return;}
 else{printf("File \"%s\" opened.\n",filename);}


 fprintf(fp,"%s\n",inttostr(width,2,32));
 fprintf(fp,"%s\n\n",inttostr(height,2,32));

 y=0;
 while(y<height)
 {
  bits=0;
  x=0;
  while(x<width)
  {
   pixel=p[x+y*width];
   r=(pixel&0xFF0000)>>16;
   g=(pixel&0x00FF00)>>8;
   b=(pixel&0x0000FF);
   gray=(r+g+b)/3;
   gray>>=8-bpp;
   bits=gray;
   fprintf(fp,"%s",inttostr(bits,16,1));
   x++;
  }
  fprintf(fp,"\n");
  y++;
 }

}
