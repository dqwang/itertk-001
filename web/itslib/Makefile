
CROSS = arm-linux-
GCC = $(CROSS)gcc
LD = $(CROSS)ld
LDFLAGS	= -r -o
EXEC = libits.a
INC	=./inc/
SRC = ./src/
OBJS	=						\
		$(SRC)itscmd.o			\
		$(SRC)net.o				\
		$(SRC)system.o			\
		$(SRC)itsip.o			\
		#$(SRC)main.o			\

.c.o:
	$(GCC) -I$(INC) -c -o $*.o $< --static

default:	$(OBJS)
	$(LD) $(LDFLAGS) $(EXEC) $(OBJS) 
copy:
	cp libits.a ../lib/
clean:
	rm -rf $(EXEC) 
	rm -rf $(SRC)*.o
