#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

/*basic input/output and memory management functions*/
#include "bbmio.h"
#include "rawio.h"
/*basic graphics functions*/
#include "bbmlib.h"

/*
 I enable or disable certain header files depending on whether they are used in the current project.
*/
/*#include "bbm_palette.h"*/
#include "bbm_font.h"
#include "bbm_gif.h"
#include "bbm_polygon.h"

/*files which contain demonstrations that make use of all the other header files.*/
#include "bbm_test.h"
#include "bbm_polygon_animation.h"
#include "bbm_palette.h"

int main(int argc, char **argv)
{
 /*bbm_palette_rainbow_test(40);*/
 bbm_palette_rainbow_anim2(40); /*makes frames for a fancy rainbow color changing checkerboard*/
 
 return 0;
}

/*
This file is part of the BBM project of Chastity White Rose/Chandler Isaac Klebs. It contains only the main function which calls other functions I have written. There are many header files I wrote to make it all work.

This and all the other source files usually come with my own makefile which has the command to compile and run them using gcc. All have been tested on msys2 under Windows 10.

If only the C standard library(The current official way):
gcc -Wall -ansi -pedantic main.c -o main && ./main

Compile with g++ and see if the code is also valid C++. (portability is important to me)
g++ -Wall -ansi -pedantic main.c -o main && ./main

Can even be compiled with tcc.
tcc main.c -o main.exe && ./main

Or even the Microsoft C compiler.
cl main.c /Fe:main && main

If compiling with SDL libraries included, the most general compile is:
gcc -Wall -ansi -pedantic main.c -o main `sdl2-config --cflags --libs` && ./main
This however has not been done since SDL code was removed.
*/
