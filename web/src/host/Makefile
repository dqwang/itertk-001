include ../../config

EXECDIR = ../../cgi-bin/
EXEC = host.cgi


OBJS	= host.o

%.o: %.c
	$(CC) -I$(INC) -g -c -o $*.o $<
default: $(OBJS)
	$(CC) $(OBJS) -o $(EXECDIR)$(EXEC) $(LIBS) $(CFLAGS) -lcrypt --static
	
clean:
	rm -f *.o
