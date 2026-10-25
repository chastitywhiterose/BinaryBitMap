/*
testing header

This file contains various tests to see if my BBM library works. It should be included as the last header before the main function.

*/


/*
Some of my functions haven't been fixed to handle situations where the width or the total image size is not a multiple of 8. The results are undefined but having the following function helps me detect when this occurs if the image doesn't look how I expect. This doesn't mean it will fail but it does mean potentially wasted bits. For best results use images which are multiples of 8 so that in the case of 1 bpp images there won't need to be extra padding bits.
*/

void warnings(int width,int height)
{
 if(width%8!=0){printf("Warning: Width is not a multiple of 8 in pixels!\n");}
 if(width*height%8!=0){printf("Warning: Image is not a multiple of 8 in total pixels!\n");}
}


/*Simple creation of an image and writing it to different formats.*/
int image_test_0()
{
 uint32_t *p=NULL; /*The pointer to the pixels*/
 uint32_t width=512,height=512; /*The size of the image.*/
 int square_size=32;

 /*for small image testing*/
 width=8;height=8;square_size=1;

 p=BBM_malloc(width,height);


 /*chastity_stripes_vertical(p,width,height,square_size);*/
 /*chastity_stripes_diagonal(p,width,height,square_size);*/
 /*chastity_stripes_horizontal(p,width,height,square_size);*/

 if(0) /*optionally change black and white to any other color if outputting 1 bpp BMP files*/
 {
  u32bw[0]=0xFF0000;
  u32bw[1]=0x00FFFF;
 }

 chastity_checker(p,width,height,square_size);

 BBM_Save_BMP(p,width,height,"./o/bbm.bmp",1);

 BBM_Save_XPM_1(p,width,height,"./o/bbm1.xpm");
 BBM_Save_XPM_3(p,width,height,"./o/bbm3.xpm");

 
 BBM_free(p);
 return 0;
}

/*
This function was made for loading images from PPM files and then writing them to other formats.
However most of the time I use the function above that creates new images of checkerboards for testing.
*/
int image_test_1()
{
 uint32_t *p=NULL; /*The pointer to the pixels*/
 uint32_t width=256,height=256; /*The size of the window.*/
 int square_size=32;

 p=BBM_malloc(width,height);
 chastity_checker(p,width,height,square_size);

 BBM_Load(&p,&width,&height,"./i/Rosalina_720.ppm");

 if(p==NULL){printf("Pointer is NULL\n"); return 0;}
 warnings(width,height);


 /*BBM_Set_BPP(p,width,height,3);*/

 BBM_Save_BMP(p,width,height,"./o/bbm.bmp",3);

 /*BBM_PAM_Test(p,width,height);*/

 BBM_free(p);
 return 0;
}

/*
This function is a test of the PAM format at different bit depths.
Most programs can't view the format but I have a way around that. The pnmtopng program I installed can convert it to PNG files which are supported by every website.
*/
int image_test_2()
{
 uint32_t *p=NULL; /*The pointer to the pixels*/
 uint32_t width=512,height=512; /*The size of the image.*/
 int square_size=32;

  /*
  u32bw[0]=0xFF0000;
  u32bw[1]=0x00FF00;
  */

/*u32bw[0]=0xFF0000;
u32bw[1]=0x00FFFF;*/

width=640;height=1136;

 p=BBM_malloc(width,height);

 chastity_checker(p,width,height,square_size);


 /*raw_load_mono(p,width,height,"./i/violinist.mono");*/
 /*raw_load_gray(p,width,height,"./i/violinist.gray");*/
 raw_load_rgb(p,width,height,"./i/violinist.rgb");

 /*raw_save_mono(p,width,height,"./o/bbm.mono");*/
 /*raw_save_gray(p,width,height,"./o/bbm.gray");*/
 /*raw_save_rgb(p,width,height,"./o/bbm.rgb");*/

 /*BBM_Save_BMP(p,width,height,"./o/bbm.bmp",24);*/

/*
 BBM_SaveTIF_BW(p,width,height,"./o/bbm1.tif");
 BBM_SaveTIF_Gray(p,width,height,"./o/bbm2.tif");
 BBM_SaveTIF_RGB(p,width,height,"./o/bbm3.tif");
*/

 BBM_SaveXBM(p,width,height,"./o/bbm.xbm");
 BBM_Save_XPM_1(p,width,height,"./o/bbm1.xpm");
 BBM_Save_XPM_3(p,width,height,"./o/bbm3.xpm");

 /*chastity_stripes_vertical(p,width,height,square_size);*/
 /*chastity_stripes_diagonal(p,width,height,square_size);*/
 /*chastity_stripes_horizontal(p,width,height,square_size);*/

 /*The NetPBM family of formats*/

 /*The first 3*/
 /*BBM_SavePBM(p,width,height,"o/bbm.pbm");
 BBM_SavePGM(p,width,height,"o/bgm.pgm");
 BBM_SavePPM(p,width,height,"o/bpm.ppm");*/

/*
 BBM_SavePAM_BW(p,width,height,"o/bbm1.pam");
 BBM_SavePAM_Gray(p,width,height,"o/bbm8.pam",8);
 BBM_SavePAM_RGB(p,width,height,"o/bbm24.pam",24);
*/

/*
 BBM_SavePAM(p,width,height,"o/bbm1.pam",1);
 BBM_SavePAM(p,width,height,"o/bbm8.pam",8);
 BBM_SavePAM(p,width,height,"o/bbm24.pam",24);

 pnmtopng("o/bbm1.pam","o/bbm1.png");
 pnmtopng("o/bbm8.pam","o/bbm8.png");
 pnmtopng("o/bbm24.pam","o/bbm24.png");
*/


 BBM_free(p);
 return 0;
}

/*
The function below is my checkerboard function which accepts command line arguments as published on the Chastity Checkerboard Project. I am preserving it here for when I need it but the main function should call this function instead.
*/
int checker(int argc, char *argv[])
{
 uint32_t *p; /*The pointer to the pixels*/
 int width=128,height=72; /*The size of the window.*/
 int square_size=32;
 char path[256];
 char filename[512];

 printf("Welcome to the checkerboard program by Chastity White Rose!\n");

 /*u32bw[0]=0xFF00FF;
 u32bw[1]=0xFFFF00;*/

 if(argc==1)
 {
  printf("A documentation may be written for this program soon!\n");
 }
 if(argc>1){sscanf(argv[1],"%i",&width);}
 if(argc>2){sscanf(argv[2],"%i",&height);}
 if(argc>3){sscanf(argv[3],"%i",&square_size);}
 /*if(argc>4){sscanf(argv[4],"%i",(int*)&u32bw[0]);}
 if(argc>5){sscanf(argv[5],"%i",(int*)&u32bw[1]);}*/

 printf("width=%d\n",width);
 printf("height=%d\n",height);
 printf("square_size=%d\n",square_size);

 p=BBM_malloc(width,height);

 chastity_checker(p,width,height,square_size);
 /*chastity_gingham(p,width,height,square_size);*/

 sprintf(path,"o/");

 /*sprintf(filename,"%schecker.bbm",path);
 BBM_Save_BBM(p,width,height,filename);*/

 /*sprintf(filename,"%schecker.tbm",path);
 BBM_SaveTBM(p,width,height,filename);*/

 sprintf(filename,"%schecker.bmp",path);
 BBM_SaveBMP_BW(p,width,height,filename);

/*
 sprintf(filename,"%schecker.pbm",path);
 BBM_SavePBM(p,width,height,filename);
*/

/*
 sprintf(filename,"%schecker.xbm",path);
 BBM_SaveXBM(p,width,height,filename);
*/

 BBM_free(p);

 /*BBM_magick("checker.bbm","checker.png");*/

 return 0;
}

/*A small function to create a checkerboard and saves it to all the currently supported monochrome bitmap formats.*/
int BBM_test_BW_write()
{
 uint32_t *p=NULL; /*The pointer to the pixels*/
 uint32_t width=8,height=8; /*The size of the image.*/
 int square_size=1;
 p=BBM_malloc(width,height);
 chastity_checker(p,width,height,square_size);
 /*
 BBM_SaveWBMP(p,width,height,"o/bbm.wbmp");
 BBM_SavePBM(p,width,height,"o/bbm.pbm");
 BBM_SaveBMP_BW(p,width,height,"o/bbm.bmp");
 BBM_Save_BBM(p,width,height,"o/bbm.bbm",1);
 */

 BBM_Save_TBM(p,width,height,"o/bbm.txt",1);

 BBM_free(p);
 return 0;
}

/*A small function to load an image and save it to all the currently officially supported monochrome bitmap formats.*/
int BBM_test_BW_read()
{
 uint32_t *p=NULL; /*The pointer to the pixels*/
 uint32_t width=12,height=16; /*The size of the image.*/
 int square_size=1;
 p=BBM_malloc(width,height);
 chastity_checker(p,width,height,square_size);

 BBM_Load(&p,&width,&height,"./i/cv/BBM_Certified-Vegan.bmp");
 if(p==NULL){printf("Pointer is NULL\n"); return 0;}

 /*BBM_Save_BBM(p,width,height,"./o/bbm.bbm",1);*/

 BBM_SaveWBMP(p,width,height,"o/bbm.wbmp");

 BBM_SaveBMP_BW(p,width,height,"o/bbm.bmp");

 /*BBM_SavePBM(p,width,height,"o/bbm.pbm");*/
 /*BBM_SavePNM(p,width,height,"o/bbm.pbm",1);*/
 /*pnmtopng("o/bbm.pbm","o/bbm.png");*/

 BBM_free(p);
 return 0;
}


/*I use this function generally for loading images that contain 24 bit color.
 Usually PPM or BMP files as they are easy to read because I worked hard writing code for them.
 The BPM in the name is a reference to the eventual BPM format I will work on but also generically refers to
 images that are full color as opposed to monochrome or grayscale.
*/
int BPM_test_read()
{
 uint32_t *p=NULL; /*The pointer to the pixels*/
 uint32_t width=12,height=16; /*The size of the image.*/
 int square_size=1;
 p=BBM_malloc(width,height);
 chastity_checker(p,width,height,square_size);

 BBM_Load(&p,&width,&height,"./o/untitled.bmp");
 if(p==NULL){printf("Pointer is NULL\n"); return 0;}

 BBM_Set_BPP(p,width,height,6);

 BBM_Save_BMP(p,width,height,"./o/bpm.bmp",24);



 BBM_free(p);
 return 0;
}



