include ../../config

EXECDIR = ../../cgi-bin/
EXEC = mode.cgi


OBJS	= mode.o

%.o: %.c
	$(CC) -I$(INC) -g  -c -o $*.o $<
default: $(OBJS)
	$(CC) $(OBJS) -o $(EXECDIR)$(EXEC) $(LIBS) -lcrypt $(CFLAGS) --static
	
clean:
	rm -f *.o
