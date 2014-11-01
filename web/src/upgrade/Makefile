include ../../config

EXECDIR = ../../cgi-bin/
EXEC = upgrade.cgi


OBJS	= upgrade.o \
		  mtdwrite.o

%.o: %.c
	$(CC) -I$(INC) -c -o $*.o $<
default: $(OBJS)
	$(CC) $(OBJS) -o $(EXECDIR)$(EXEC) $(LIBS) -lcrypt $(CFLAGS)
	
clean:
	rm -f *.o
