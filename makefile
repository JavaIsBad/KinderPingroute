CFLAGS = -g -Wall -std=gnu99 -pedantic -Werror -W -Wextra -Wmissing-declarations -Wmissing-prototypes -Wredundant-decls -Wshadow -Wbad-function-cast -Wcast-qual
OPATH = obj/
IFLAGS = -Iinclude/
LIBFLAGS = -lrt -pthread

vpath %.c src/
vpath %.h include/
vpath %.o obj/
vpath ping bin/

ping : ping.o pingICMP.o pingTCP.o timeuh.o tools.o | bin
	gcc $(CFLAGS) -o ping $(OPATH)ping.o $(OPATH)timeuh.o $(OPATH)pingICMP.o $(OPATH)tools.o $(OPATH)pingTCP.o $(LIBFLAGS)
	mv $@ bin/

ping.o : ping.c ping.h
timeuh.o : timeuh.c timeuh.h
pingICMP.o : pingICMP.c pingICMP.h
pingTCP.o : pingTCP.c pingTCP.h
tools.o : tools.c tools.h

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
