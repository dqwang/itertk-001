
CROSS = arm-linux-uclibc-
GCC = $(CROSS)gcc
EXEC = app
INC	=./inc/
SRC = ./src/
OBJ = ./obj/
OBJS	=						\
		$(OBJ)main.o			\
		$(OBJ)system.o			\
		$(OBJ)log.o				\
		$(OBJ)file.o			\
		$(OBJ)config.o			\
		$(OBJ)thread.o			\
		$(OBJ)web_net.o			\
		$(OBJ)itsip.o			\
		$(OBJ)com.o				\
		$(OBJ)mode.o			\

$(OBJ)%.o: $(SRC)%.c $(INC)*.h
	$(GCC) -Wall  -I$(INC) -g -c -o $(OBJ)$*.o $<

default:	$(OBJS)
	$(GCC) -O0 -o $(EXEC) $(OBJS) -lpthread -lcrypt --static
	cp $(EXEC) /home/jack/n32926/bsp/tftp
	cp $(EXEC) /home/jack/n32926/bsp/tftp/image/bin/
	cp $(EXEC) /home/jack/n32926/bsp/tftp/start/nand1-2/app/bin/
clean:
	rm -rf $(EXEC) 
	rm -rf $(OBJ)*.o
