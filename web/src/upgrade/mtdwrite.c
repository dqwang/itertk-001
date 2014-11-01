#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <linux/compiler.h>
#include <mtd/mtd-user.h>
#include <string.h>

mtd_info_t meminfo;

int mtdinfo_read(char* dev)
{
	int Fd = open(dev, O_RDONLY);
	if(Fd <= 0)
	{
		perror("MTD open");
		return -1;
	}

	if (ioctl(Fd,MEMGETINFO,&meminfo) == 0)
	{
		//printf("size = %x\n", meminfo.size);
		//printf("erasesize = %x\n", meminfo.erasesize);
		//printf("writesize = %x\n", meminfo.writesize);
	}
	else
	{
		perror("ioctl");
		close(Fd);
		return -1;
	}

	return 0;
}

int non_region_erase(char * dev, int start, int count)
{
	
	int Fd = open(dev, O_RDWR);
	if(Fd <= 0)
	{
		perror("open");
		return -1;
	}

    
	erase_info_t erase;
	erase.start = start;
	erase.length = meminfo.erasesize;

	for(; count > 0; count --)
	{
		/*	
		printf("Performing Flash unlock at offset 0x%x",erase.start);
		if(ioctl(Fd, MEMUNLOCK, &erase) != 0)
		{
			perror("MTD Unlock failure");
			close(Fd);
			return -1;
		}
		*/

//		printf("Performing Flash Erase of length %u at offset 0x%x\n",
//				erase.length, erase.start);
//		fflush(stdout);
		if(ioctl(Fd, MEMERASE, &erase) != 0)
		{
			perror("MTD Erase failure");
			close(Fd);
			return -1;
		}
		erase.start += meminfo.erasesize;
	}
//	printf("MTD erase done\n");
	close(Fd);
	return 0;
}

int mtd_write(char* dev, char* file)
{
	int file_len;
	int ifd = open(file, O_RDONLY);
	if(ifd <= 0)
	{
		perror("open file failure");
		return -1;
	}
	else
	{
		file_len = lseek(ifd, 0, SEEK_END);
		lseek(ifd, 0, SEEK_SET);
//		printf("image len is %d \n", file_len);
	}

	int fd = open(dev, O_RDWR);

	int readlen = meminfo.writesize;
	char * write_buf = (char*)malloc(readlen);
	if(write_buf == NULL)
	{
		perror("malloc");
		close(ifd);
		close(fd);
		return -1;
	}
	
//	printf("trace\n");
	int imglen = file_len;
	int cnt;
//	printf("file_len = %d\n", file_len);
	while(file_len)
	{
		memset(write_buf, 0xff, readlen);
		if((cnt = read(ifd, write_buf, readlen)) != readlen)
		{
//			printf("the file is end!\n");
		}

		int write_cnt; 
		if((write_cnt = write(fd, write_buf, readlen)) != readlen)
		{
			printf("write mtd device error! startaddr=%d\n",(imglen-file_len));
			close(ifd);
			close(fd);
			return -1;
		}
		else
		{
//			printf("\rwrite mtd device ok!startaddr=%d",(imglen-file_len));	
			file_len -= cnt;
		}
	}
//	printf("\n");
	close(ifd);
	close(fd);
	return 0;
}	

/*
int main(int argc, char* argv[])
{
	int ret = mtdinfo_read("/dev/mtd1");
	if(ret < 0)
	{
		printf("read mtd info failed!\n");
		return -1;
	}
	non_region_erase("/dev/mtd1", 0, (meminfo.size/meminfo.erasesize));

	mtd_write("/dev/mtd1", "4.cramfs");
	return 0;
}
*/
