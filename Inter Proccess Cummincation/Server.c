#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#define SHAREDMEMORY 1
#define SOCKET 2
#define PORT 1000
#define MAX_MES_SIZE 36
#define TRUE 1

void shareMemory();

void Socket();
int SetServer();

size_t writeFile(char *, int);

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		fprintf(stderr, "Please Provide valid arguments\n");
		exit(EXIT_FAILURE);
	}
	int choice = atoi(argv[1]);

	switch (choice)
	{
	case SHAREDMEMORY:
		shareMemory();
		break;
	case SOCKET:
		Socket();
		break;
	default:
		fprintf(stderr, "1 and 2 are the only valid arguments");
		exit(EXIT_FAILURE);
	}

	return 0;
}

void shareMemory()
{
	struct sembuf ops;					
	void *shm_address;					
	int semid, shmid, rc;				
	key_t shmkey = 1111, semkey = 9999; 

	semid = semget(semkey, 1, IPC_CREAT);
	if (semid == -1)
	{
		fprintf(stderr, "semaphore id failed to get :%s \n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (semctl(semid, 0, SETVAL, 1) == -1) 
	{
		fprintf(stderr, "semaphore control error:%s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	shmid = shmget(shmkey, 32, IPC_CREAT);
    
	
	if (shmid == -1)
	{
		fprintf(stderr, "Shared memory id failed to get: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	shm_address = shmat(shmid, NULL, 0);

	if (shm_address == NULL)
	{
		fprintf(stderr, "Failed to attach Shared Memory to your process:%s \n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	while (1)
	{

		printf("%s", (char *)shm_address);
		writeFile((char *)shm_address, SHAREDMEMORY);
        
		
		ops.sem_num = 0;
		ops.sem_op = -1;
		ops.sem_flg = 0;

		rc = semop(semid, &ops, 1);
		if (rc == -1)
		{
			fprintf(stderr, "Semaphore Operation setting failed%s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		if (strncmp(shm_address, "exit", 4) == 0)
		{
			break;
		}
	}
	
	rc = shmdt(shm_address);
	if (rc == -1)
	{
		printf(" Shared Memory detach failed:%s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

    
	if (shmctl(shmid, IPC_RMID, NULL) == -1)
	{
		fprintf(stderr, "Error in Closing Shared Memory section:%s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void Socket()
{
     
	int Connection = SetServer();
	char msg[MAX_MES_SIZE];
	char size[10];

	while (TRUE)
	{

		memset(msg, 0, MAX_MES_SIZE);
		read(Connection, msg, MAX_MES_SIZE);
		printf(">From Client: %s \n", msg);

		int len = writeFile(msg, SOCKET);
		snprintf(size, 10, "%lu", (unsigned long)len);

		write(Connection, size, sizeof(msg));

		if (strncmp(msg, "exit", 4) == 0)
		{
			break;
		}
	}

	close(Connection);
}

int SetServer()
{
	int tcp_sock = 0, Connnection = 0, len = 0;

	struct sockaddr_in server_addr, Cli_addr;

	tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (tcp_sock == -1)
	{
		printf("Socket Creation Failed...\n");
		exit(0);
	}
	else
		printf("Socket Creation Successfull...\n");
	bzero((struct sockaddr *)&server_addr, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORT);

	if ((bind(tcp_sock, (struct sockaddr *)&server_addr, sizeof(server_addr))) != 0)
	{
		printf("Socket Binding Failed..\n");
		exit(0);
	}
	else
	{
		printf("Socket Binded Successfully..\n");
	}
    
	if ((listen(tcp_sock, 5)) != 0)
	{
		printf("Listen Failed.\n");
		exit(0);
	}
	else
	{
		printf("Server Listening..\n");
	}
	len = sizeof(Cli_addr);

	Connnection = accept(tcp_sock, (struct sockaddr *)&Cli_addr, &len);

	if (Connnection < 0)
	{
		printf("Server Accept Failed.\n");
		exit(0);
	}
	else
	{
		printf(">Server accept the Client...\n>");
	}

	return Connnection;
}

size_t writeFile(char *msg, int type)
{

	char header[36];
	
	if (type == SHAREDMEMORY)
	{
		strcpy(header, "Communication with Shared memeory\n");

		int fd = open("Chat 1", O_CREAT | O_RDWR);
		if (fd == -1)
		{
			fprintf(stderr, "Unable to create file with this name %s %s\n", "Chat", strerror(errno));
			exit(EXIT_FAILURE);
		}

		int wb = write(fd, header, 34);
		if (wb != 34)
		{
			fprintf(stderr, "write:%s:%s\n", "Chat",
					strerror(errno));
			exit(EXIT_FAILURE);
		}

		int offset = lseek(fd, SEEK_SET, SEEK_END);
		if (offset == -1)
		{
			fprintf(stderr, "lseek:%s:%s\n", "Chat",
					strerror(errno));
			exit(EXIT_FAILURE);
		}

		int size = strlen(msg);
		wb = write(fd, msg, size);
		if (wb != size)
		{
			fprintf(stderr, "write:%s:%s\n", "Chat",
					strerror(errno));
			exit(EXIT_FAILURE);
		}

		if (close(fd) == -1)
		{
			fprintf(stderr, "close:%s:%s\n", "Chat",
					strerror(errno));
			exit(EXIT_FAILURE);
		}
		return (offset + size);
	}
	
	if (type == SOCKET)
	{
		strcpy(header, "Communication with Socket\n");

		int fd = open("Chat 2", O_CREAT | O_RDWR);
		if (fd == -1)
		{
			fprintf(stderr, "Unable to create file with this name %s %s\n", "Chat", strerror(errno));
			exit(EXIT_FAILURE);
		}
		int wb = write(fd, header, 26);
		if (wb != 26)
		{
			fprintf(stderr, "write:%s:%s\n", "Chat",
					strerror(errno));
			exit(EXIT_FAILURE);
		}
		int offset = lseek(fd, SEEK_SET, SEEK_END);
		if (offset == -1)
		{
			fprintf(stderr, "lseek:%s:%s\n", "Chat",
					strerror(errno));
			exit(EXIT_FAILURE);
		}

		int size = strlen(msg);
		wb = write(fd, msg, size);
		if (wb != size)
		{
			fprintf(stderr, "write:%s:%s\n", "Chat",
					strerror(errno));
			exit(EXIT_FAILURE);
		}

		if (close(fd) == -1)
		{
			fprintf(stderr, "close:%s:%s\n", "Chat",
					strerror(errno));
			exit(EXIT_FAILURE);
		}
		return (offset + size);
	}
}
