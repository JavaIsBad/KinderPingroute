CFLAGS = -Wall -std=gnu99 -pedantic -Werror -W -Wextra -Wmissing-declarations -Wmissing-prototypes -Wredundant-decls -Wshadow -Wbad-function-cast -Wcast-qual
OPATH = obj/
IFLAGS = -Iinclude/
LIBFLAGS = -lrt -pthread

vpath %.c src/
vpath %.h include/
vpath %.o obj/

ping : ping.o timeuh.o | bin
	gcc $(CFLAGS) -o ping $(OPATH)ping.o $(OPATH)timeuh.o $(LIBFLAGS)
	mv $@ bin/

ping.o : ping.c ping.h
timeuh.o : timeuh.c timeuh.h

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
