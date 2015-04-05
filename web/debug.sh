cd /home/jack/n32926/bsp/itertk-001/itslib
make clean
make
make copy

echo "sleep 3"
sleep 3

cd /home/jack/n32926/bsp/itertk-001/web
make clean
make
make copy

echo "sleep 3"
sleep 3

echo "copy htmls to tftp and nand"
cd html/;./debug.sh;cd ../

echo "sleep 3"
sleep 3

echo "generate update.tar.gz"
cd /home/jack/n32926/bsp/itertk-001/update
./update_app.sh

