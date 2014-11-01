include ../../config

EXECDIR = ../../cgi-bin/
EXEC = top.cgi


OBJS	= top.o

%.o: %.c
	$(CC) -I$(INC) -c -o $*.o $<
default: $(OBJS)
	$(CC) $(OBJS) -o $(EXECDIR)$(EXEC) $(LIBS) --static -lcrypt
	
clean:
	rm -f *.o
