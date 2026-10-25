std:
	g++ -Wall -ansi -pedantic main.cpp -o main && ./main
x264:
	ffmpeg -r 30 -f image2 -i o/%08d.bmp -vcodec libx264 -preset veryslow -crf 23 -pix_fmt yuv420p o/v.mp4
x256:
	ffmpeg -r 30 -f image2 -i o/%08d.bmp -vcodec libx265 -preset veryslow -crf 28 -pix_fmt yuv420p o/v.mp4
gif:
	gm convert -delay 5 -loop 0 o/*.bmp o/0.gif
clean:
	rm main
	rm o/*
