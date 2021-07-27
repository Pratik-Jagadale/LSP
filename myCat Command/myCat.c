#include <fcntl.h>
#include <sys/types.h>
#include <stdlib.h>

#define BUF_SIZE 1024

int main(int argc,char* argv[]){
	int fd;
	char buf[BUF_SIZE];

	if(argc != 2){
		printf("Usage : myCat filen %s \n",argv[1]);
		exit(0);
	}

	fd = open(argv[1],ORDONLY);

	if(fd == -1){
		printf("Unable to open %s\n",argv[1]);
		exit(2);
	}

	//int cnt = read(fd, buf, BUF_SIZE);

	while((cnt = read(fd,BUF_SIZE)) > 0){ //read returns the count of which no of byte readed...
		write(1, buf, cnt);
	}

	close(fd);
	exit(0);
}
