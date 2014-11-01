include ../../config

EXECDIR = ../../cgi-bin/
EXEC = com.cgi


OBJS	= com.o

%.o: %.c
	$(CC) -I$(INC) -g  -c -o $*.o $<
default: $(OBJS)
	$(CC) $(OBJS) -o $(EXECDIR)$(EXEC) $(LIBS) $(CFLAGS) -lcrypt
	
clean:
	rm -f *.o
