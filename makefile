CFLAGS = -Wall -std=gnu99 -pedantic -Werror -W -Wextra -Wmissing-declarations -Wmissing-prototypes -Wreduntand-decls -Wshadow -Wbad-function-cast -Wcast-qual
OPATH = obj/
IFLAGS = -Iinclude/

vpath %.c src/
vpath %.h include/
vpath %.o obj/

ping : ping.o | bin
	gcc $(CFLAGS) -o ping $(OPATH)ping.o
	mv $@ bin/

ping.o : ping.c ping.h
traceroute.o :

%.o : | obj
	gcc $(CFLAGS) -c $< $(IFLAGS)
	mv $@ $(OPATH)
	
obj :
	mkdir obj

bin :
	mkdir bin

clean :
	rm -r obj/ bin/ doc/

doc :
	@doxygen
