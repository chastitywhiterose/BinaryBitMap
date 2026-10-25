/*
This is only a testing file which uses my BinaryBitMap class to achieve certain animations. The C++ rewrite of my code was written in mind for animations more so than still pictures like the C version was originally.
*/

/*
This is a simple spinning polygon animation. Well actually it's not so simple because a lot of code went into making it work, but it looks simple as the end result.
*/
void bbmpp_test_0()
{
 int width=1280,height=720,square_size=16,sides=3,fps=30;
 BinaryBitMap bbm(width,height),bbm1(width,height);

 bbm.info();
 bbm.fill_checker(square_size,0x000000,0xFFFFFF);

 frame=0;
 while(frame < fps*60 /* fps*60 */)
 {
  bbm1.fill(0x000000);
  bbm1.polygon(bbm1.width/2,bbm1.height/2,bbm1.height*7/16,radians,sides,0xFFFFFF);
  bbm1.scan_fill();

  radians+=M_PI/180; /*change rotation*/

  /*the XOR section*/
  int x,y;
  y=0;
  while(y<height)
  {
   x=0;
   while(x<width)
   {
    bbm1.pointer[x+y*width]=bbm.pointer[x+y*width] ^ bbm1.pointer[x+y*width];
    x+=1;
   }
   y+=1;
  }


  bbm1.save_frame();

  if(frame%(fps*10)==0){sides+=1;}

 }

}


/*
This test involves loading an existing image and doing things to it.
*/
void bbmpp_test_1()
{
 int square_size=16,sides=15,fps=30;
 BinaryBitMap bbm(2000,2000);

 bbm.info();
 bbm.fill_checker(square_size,0x000000,0xFFFFFF);

 bbm.load_bmp("./i/0.bmp");

 frame=0;
 while(frame < 1 /* fps*60 */)
 {
  //bbm.fill(0x000000);
  //bbm.polyline(bbm.width/2,bbm.height/2,bbm.height*7/16,radians,sides,0xFFFFFF);
  //bbm.star(bbm.width/2,bbm.height/2,bbm.height*7/16,radians,sides,7,0xFFFFFF);
  //bbm.polygon(bbm.width/2,bbm.height/2,bbm.height*7/16,radians,sides,0xFFFFFF);

  radians+=M_PI/180; /*change rotation*/

  bbm.save_frame();

  if(frame%(fps*10)==0){sides+=1;}

 }

}