if [ -f nand1-2.tar.gz ];then
	rm nand1-2.tar.gz
fi
	
date > /home/jack/n32926/bsp/tftp/start/nand1-2/update_history
cd /home/jack/n32926/bsp/tftp/start/nand1-2
ls 

#don't modify the configuration on the directory : data/
#tar -czvf nand1-2.tar.gz app  boa  data  update_history
tar -czvf nand1-2.tar.gz app boa update_history

mv nand1-2.tar.gz /home/jack/n32926/bsp/itertk-001/update

pwd
echo "-------------------------"
echo "Create nand1-2.tar.gz: ok"
echo "::::::::::::::::::::::::)"





