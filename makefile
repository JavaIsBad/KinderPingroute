CFLAGS = -g -Wall -std=gnu99 -pedantic -Werror -W -Wextra -Wmissing-declarations -Wmissing-prototypes -Wredundant-decls -Wshadow -Wbad-function-cast -Wcast-qual
OPATH = obj/
IFLAGS = -Iinclude/
LIBFLAGS = -lrt -pthread

vpath %.c src/
vpath %.h include/
vpath %.o obj/
vpath ping bin/
vpath traceroute bin/

all : ping traceroute
	

ping : ping.o pingICMP.o pingTCP.o pingUDP.o timeuh.o tools.o | bin
	gcc $(CFLAGS) -o ping $(OPATH)ping.o $(OPATH)timeuh.o $(OPATH)pingICMP.o $(OPATH)tools.o $(OPATH)pingUDP.o $(OPATH)pingTCP.o $(LIBFLAGS)
	mv $@ bin/

traceroute : traceroute.o tracerouteICMP.o tracerouteUDP.o tracerouteTCP.o timeuh.o tools.o | bin
	gcc $(CFLAGS) -o traceroute $(OPATH)traceroute.o $(OPATH)tracerouteTCP.o $(OPATH)tracerouteUDP.o $(OPATH)tracerouteICMP.o $(OPATH)timeuh.o $(OPATH)tools.o
	mv $@ bin/

ping.o : ping.c ping.h const.h timeuh.o tools.o
timeuh.o : timeuh.c timeuh.h const.h
pingICMP.o : pingICMP.c pingICMP.h const.h timeuh.o tools.o
pingTCP.o : pingTCP.c pingTCP.h const.h timeuh.o tools.o
pingUDP.o : pingUDP.c pingUDP.h const.h timeuh.o tools.o
tools.o : tools.c tools.h const.h timeuh.o
traceroute.o : traceroute.c traceroute.h const.h timeuh.o tools.o
tracerouteICMP.o :tracerouteICMP.c tracerouteICMP.h const.h timeuh.o tools.o
tracerouteUDP.o : tracerouteUDP.c tracerouteUDP.h const.h timeuh.o tools.o
tracerouteTCP.o : tracerouteTCP.c tracerouteTCP.h const.h timeuh.o tools.o

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
