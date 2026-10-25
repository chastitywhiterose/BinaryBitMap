Chastity checkerboard project

I am Chastity White Rose AKA Chandler Klebs. The master of checkerboards and lover of polygons and ponies!

I started a new project to make a program that creates a checkerboard, displays it to the screen until a user presses enter, and then saves it to a file and exits. That was the start. I have made some progress since then and added small other features.

For this project I'm leaving out the fancy animation stuff. The idea is to focus on the creation of the images themselves and I also wrote code to produce other formats. The SDL library itself can write BMP files but I can also do things like PBM,PGM,PPM, and PAM.

It has only the minimum functions required and much was copied from my existing codebase. This could be in a book someday. For now I created this project so I could share my code with people who want to learn from it.

Version 1 is already complete and you will find it here:
https://sourceforge.net/projects/chastity-checkerboard/files/releases/chastity%20checker%20version%201/

It's fully functional but that doesn't mean my work is over! I am in the process of creating my own image format and eventually writing a set of tools to convert my format into other formats. It will be sort of similar to the Netpbm project but this is my own hobby project as I take on the challenge of creating a format that I believe is superior to the existing monochrome formats. Of course that's a matter of opinion but I'll learn a lot more computer programming by doing it my own way.

The following are links I required for my research while writing all this C code. I needed info on the formats and also a tool to convert them as needed.

http://netpbm.sourceforge.net/doc/#formats
https://ffmpeg.org/general.html#Image-Formats
https://en.wikipedia.org/wiki/BMP_file_format
https://imagemagick.org/script/formats.php
http://www.graphicsmagick.org/formats.html
https://en.wikipedia.org/wiki/X_BitMap
https://en.wikipedia.org/wiki/Wireless_Application_Protocol_Bitmap_Format

https://en.wikipedia.org/wiki/Free_Lossless_Image_Format