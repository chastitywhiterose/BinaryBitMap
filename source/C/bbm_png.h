/*
This file is dedicated to the PNG format. There are a variety of methods which can be used to convert the other formats from my project into the png format. ImageMagick, GraphicsMagick, and the netpbm tools can be used as well as most image viewers or editors.

The PNG format allows lossless compression and as a result is quite useful with limited resources. However it's not simple enough to write a function myself that reads or writes it. That's why I use third party command line tools to do the dirtywork.

I will be providing more links and possible functions which are useful in the event I need them.

http://netpbm.sourceforge.net/doc/pnmtopng.html
https://imagemagick.org/script/formats.php
http://www.graphicsmagick.org/formats.html

*/

/*
I decided to install the netpbm series of tools because they allow conversion between the existing PBM,PGM, and PPM formats that my program can already write and other formats like PNG. ImageMagick and GraphicsMagick also can but I want to have all the tools so that I can compare the difference in the PNG files they produce and see which is smaller. The entire point of PNG is small file sizes. If I want large file sizes, that's what I already have with PPM and BMP!

http://netpbm.sourceforge.net/getting_netpbm.php
*/

/*
This is a simple function that runs an external command to convert an existing file into the PNG format.
However, because it currently is set up to use gm from the graphicsmagick suite, it is not limited to PNG.
It also deletes the original file after it is no longer needed.
*/
void BBM_convert_PNG(const char* infile,const char* outfile)
{
 char command[256];
 sprintf(command,"gm convert %s %s",infile,outfile);
 /*printf("%s\n",command);*/
 system(command);
 if( remove( infile ) != 0 ){printf("could not delete file %s\n",infile);}
}
