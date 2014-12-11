all:
	cd src/com;make
	cd src/host;make
	cd src/mainFrame;make
	cd src/net;make
	cd src/top;make
#	cd src/user;make
	cd src/factory;make
#	cd src/user;make
#	cd src/issue;make
	cd src/mode;make
#	cd src/issue;make
#	cd src/reset;make
	cd src/upgrade;make
	cd src/vlan;make
	cd src/gpio;make
	cd src/server;make
clean:
	cd src/com;make clean
	cd src/host;make clean
	cd src/mainFrame;make clean
	cd src/net;make clean
	cd src/top;make clean
#	cd src/user;make clean
	cd src/factory;make clean
#	cd src/issue;make clean
	cd src/mode;make clean
#	cd src/reset;make clean
	cd src/upgrade;make clean
#	cd src/vlan;make clean
	cd src/gpio;make clean
	cd src/server;make clean

copy:
	cp cgi-bin/*.cgi /home/jack/n32926/bsp/tftp/start/nand1-2/app/web/cgi-bin
	cp cgi-bin/*.cgi /home/jack/n32926/bsp/tftp
