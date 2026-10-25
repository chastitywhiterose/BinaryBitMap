#This is my makefile for conveniently making several kinds of things.
std:
	gcc -Wall -ansi -pedantic main.c -o main -lm && ./main
std_cpp:
	g++ -Wall -ansi -pedantic main.c -o main -lm && ./main
SDL:
	gcc -Wall -ansi -pedantic main.c -o main `sdl2-config --cflags --libs` && ./main
SDL_cpp:
	g++ -Wall -ansi -pedantic main.c -o main `sdl2-config --cflags --libs` && ./main
tcc:
	tcc main.c -o main -lm && ./main
x264:
	ffmpeg -r 30 -f image2 -i o/%08d.bmp -vcodec libx264 -preset veryslow -crf 23 -pix_fmt yuv420p o/v.mp4
x256:
	ffmpeg -r 30 -f image2 -i o/%08d.bmp -vcodec libx265 -preset veryslow -crf 28 -pix_fmt yuv420p o/v.mp4
gif:
	magick convert -delay 5 -loop 0 o/*.bmp o/0.gif
gmgif:
	gm convert -delay 5 -loop 0 o/*.bmp o/0.gif
pngtogif:
	magick convert -delay 5 -loop 0 o/*.png o/video.gif
hello:
	echo Hello World!
backup:
	tar -cvzf ./o/bbm.tar.gz *.c *.h makefile
backup-frames:
	tar -cvzf bbm_frames.tar.gz ./o
clean-frames:
	rm o/*.bmp
clean:
	rm main
	rm o/*
