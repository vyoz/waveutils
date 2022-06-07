
all:
	g++ -o wavefile wavefile.cpp
	gcc -o pcm2wave pcm2wave.c

clean:
	rm -f pcm2wave wavefile
