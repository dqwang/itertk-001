cd /home/jack/n32926/bsp/itertk-001/itslib
make clean
echo "----------------------itslib clean done-------------------------------------"
make
echo "----------------------itslib make done-------------------------------------"
sleep 2s
make copy
echo "----------------------itslib copy done-------------------------------------"


#echo "sleep 3"
#sleep 3

cd /home/jack/n32926/bsp/itertk-001/web
make clean
echo "----------------------web clean done-------------------------------------"
make
echo "----------------------web make  done-------------------------------------"
sleep 2s
make copy
echo "----------------------web copy  done-------------------------------------"

#echo "sleep 3"
#sleep 3

echo "---------------------copy htmls to tftp and nand done--------------------"
cd html/;./debug.sh;cd ../

#echo "sleep 3"
#sleep 3

echo "---------------------generate update.tar.gz-------------------------------"
cd /home/jack/n32926/bsp/itertk-001/update
./update_app.sh

