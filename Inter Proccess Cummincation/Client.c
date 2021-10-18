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

#define PORTNO 1000
#define SHAREDMEMORY 1
#define SOCKET 2
#define TRUE 1
#define MAX_MESSAGE_SIZE 36

void share_memory();

void Socket();
int setClient();

int main(int argc, char *argv[])
{
	if (argc != 2) 
	{
		fprintf(stderr, "Please Enter valid arguments\n");
		exit(EXIT_FAILURE);
	}
	int choice = atoi(argv[1]);

	switch (choice)
	{
	case SHAREDMEMORY:
		share_memory();
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

void share_memory()
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

		char msg[32] = "\0";
		printf("Enter message\n>");
		fgets(msg, 32, stdin);

		strcpy((char *)shm_address, msg);

		ops.sem_num = 0;
		ops.sem_op = +1;
		ops.sem_flg = 0;

		rc = semop(semid, &ops, 1);
		if (rc == -1)
		{
			fprintf(stderr, "Semaphore Operation setting failed%s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		if (strncmp(msg, "exit", 4) == 0)
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
}

void Socket()
{
	int Connection = setClient();

	char msg[MAX_MESSAGE_SIZE];

	while (TRUE)
	{
		memset(msg, 0, MAX_MESSAGE_SIZE);
		printf("\nEnter Message client to server\n>");
		fgets(msg, MAX_MESSAGE_SIZE, stdin);
		write(Connection, msg, strlen(msg));
		if (strncmp(msg, "exit", 4) == 0)
		{
			break;
		}
		memset(msg, 0, MAX_MESSAGE_SIZE);
		read(Connection, msg, MAX_MESSAGE_SIZE);
		printf(" \t Message From Server [%s]", msg);
	}
	close(Connection);
}

int setClient()
{

	int tcp_sock, Connection;
	struct sockaddr_in server_addr, Cli_addr;

	tcp_sock = socket(AF_INET, SOCK_STREAM, 0);

	if (tcp_sock == -1)
	{
		printf("Socket Creation Failed..\n");
		exit(0);
	}
	else
	{
		printf("Socket Successfully Created..\n");
	}
	bzero(&server_addr, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORTNO);

	if (connect(tcp_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0)
	{
		printf("Connection with the server failed.\n");
		exit(0);
	}
	else
		printf("Connected To the Server..\n");

	return (tcp_sock);
}

/*
1.Shared Memory IPC command-> "./Server 1" and then "./Clinet 1"   
2.Socket IPC command-> "./Server 2" and then "./Clinet 2" 
*/
