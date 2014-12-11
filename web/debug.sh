cd /home/jack/n32926/bsp/itertk-001/itslib
make clean
make
make copy

sleep 1

cd /home/jack/n32926/bsp/itertk-001/web
make clean
make
make copy

sleep 1

echo "copy htmls to tftp and nand"
cd html/;./debug.sh;cd ../
