My source code is split into different branches under these folders.

bbm         : the original ANSI C source from which all the others are based on.
bbmpp       : the C Plus Plus version which uses a class I wrote but basically works the same way as bbm
bbm_allegro : Using the C allegro library to do things like drawing text that my main bbm branch cannot do
rust        : I experimented with the Rust Programming Language but never

The tar.gz files in these folders are backups I made with the Unix tar command. I could have used zip but I wanted to have that Unix feeling.

For example the following command:

tar -cvzf ~/tmp/bbm.tar.gz *.c *.h makefile

Makes a backup of all my c source and header files along with the makefile I used to compile them. Sometimes I'll include the date in the name of the file if I need to make a certain version of my code permanently available.

Other times I may just upload the files without compressing them in an archive.
