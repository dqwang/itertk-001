
CROSS = arm-linux-
GCC = $(CROSS)gcc
LD = $(CROSS)ld
LDFLAGS	= -r -o
EXEC = libits-arm.a
INC	=./inc/
SRC = ./src/
OBJS	=						\
		$(SRC)itscmd.o			\
		$(SRC)net.o				\
		$(SRC)system.o			\
		$(SRC)itsip.o			\
		#$(SRC)main.o			\

.c.o:
	$(GCC) -I$(INC) -c -o $*.o $<

default:	$(OBJS)
	$(LD) $(LDFLAGS) $(EXEC) $(OBJS) 
clean:
	rm -rf $(EXEC) 
	rm -rf $(SRC)*.o
