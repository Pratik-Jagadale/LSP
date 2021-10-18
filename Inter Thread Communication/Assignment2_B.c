#include <stdio.h> 
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>

#define TRUE 1
#define EXIT 2
#define READ 1
#define KEY 8888
#define SLEEP_TIME 2
#define MAX_BUFFER_SIZE 32

struct message
{

	int type;
	char msg[MAX_BUFFER_SIZE];
};

typedef struct message Message_t;

void *senderThreadFun(void *p_input);
void *receiverThreadFun(void *p_input);
void flushMessage(Message_t *p_message);

int main()
{
	pthread_t sender_id;
	pthread_t receive_id;

	pthread_create(&sender_id, NULL, senderThreadFun, NULL);
	pthread_create(&receive_id, NULL, receiverThreadFun, (void *)receive_id);

	pthread_join(sender_id, NULL);
	pthread_join(receive_id, NULL);

	return (EXIT_SUCCESS);
}

void *senderThreadFun(void *p_input)
{

	int msgid;
	int send_ret;
	Message_t message;

	msgid = msgget(KEY, IPC_CREAT);
	if (msgid == -1)
	{
		fprintf(stderr, "msgget:%s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	while (TRUE)
	{
		printf("Enter Message Pass\n>");
		if (fgets(message.msg, MAX_BUFFER_SIZE, stdin) != NULL)
		{
		}

		if (strncmp(message.msg, "exit", 4) == 0)
		{
			message.type = EXIT;
			send_ret = msgsnd(msgid, (void *)&message, sizeof(Message_t), 0);
			if (send_ret == -1)
			{
				fprintf(stderr, "message sending error:%s", strerror(errno));
				exit(EXIT_FAILURE);
			}

			return NULL;
		}
		else if (isspace(message.msg[0]))
		{
		}
		else
		{
			message.type = READ;
		}
		send_ret = msgsnd(msgid, (void *)&message, sizeof(Message_t), 0);

		if (send_ret == -1)
		{
			fprintf(stderr, "message sending error:%s", strerror(errno));
			exit(EXIT_FAILURE);
		}
		flushMessage(&message);
	}
}
void *receiverThreadFun(void *input)
{

	char file_name[10];
	snprintf(file_name, 10, "%lu", (pthread_t)input);

	Message_t message;
	int msgid;
	int recv_ret;
	int wb;

	msgid = msgget(KEY, IPC_CREAT);
	if (msgid == -1)
	{
		fprintf(stderr, "msgget:%s\n", strerror(errno));
	}

	int fd = open(file_name, O_CREAT | O_RDWR);
	if (fd == -1)
	{
		fprintf(stderr, "Unable to create file with this name %s %s\n", file_name, strerror(errno));
		exit(EXIT_FAILURE);
	}

	while (TRUE)
	{
		flushMessage(&message);
		recv_ret = msgrcv(msgid, (void *)&message, sizeof(Message_t), 0, 0);
		if (recv_ret == -1)
		{
			fprintf(stderr, "Message Receiving error %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		int n = strlen(message.msg);
		switch (message.type)
		{

		case EXIT:
			wb = write(fd, message.msg, n);

			if (wb != n)
			{
				fprintf(stderr, "Write failed %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}

			if (msgctl(msgid, IPC_RMID, NULL) == -1)
			{
				fprintf(stderr, "Closing queue error %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}

			if (close(fd) == -1)
			{
				fprintf(stderr, "File closing error %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}
			printf("\nExiting Thank you\n");
			return NULL;

		case READ:
			wb = write(fd, message.msg, n);
			if (wb != n)
			{
				fprintf(stderr, "Read error %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}
			break;
		case 0:
			printf("\t\t\tIn sleep\n");
			sleep(SLEEP_TIME);
			printf("\t\t\tOut form sleep\n>");
			break;
		}
	}
}

void flushMessage(Message_t *message)
{
	memset(message->msg, '\0', MAX_BUFFER_SIZE);
	message->type = 0;
}
