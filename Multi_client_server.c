#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include  <sys/ioctl.h>

#define SIZE 10

void init_sock (void);
int connfd = 0;
///////////////////////////////////////////////////////
int server(void)
{
	int sockfd = 0, n = 0;
	char recvBuff[1024] = {'\0'};
	struct sockaddr_in serv_addr;
	FILE *fp;
	int fd;

	printf("serv 1\n");
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\n Error : Could not create socket \n");
		return 1;
	}
	printf("serv 2\n");
	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(5000);
	printf("serv 3\n");
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
	{
		printf("\n inet_pton error occurred\n");
		return EXIT_FAILURE;
	}
	printf("serv 4\n");
	if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("\n Error : Server Connect Failed \n");
		return EXIT_FAILURE;
	}
	printf("connfd server = %d\n",connfd);
	printf("serv 5\n");
	printf("n0 = %d\n", n);
    int len = 0;

    /*while(1)
	{
		ioctl(sockfd, FIONREAD, &len);
		printf("len = %d\n", len);
		sleep(1);
	}*/

	while ((n = read(sockfd, recvBuff, sizeof(recvBuff) - 1)) > 0)
	{
		printf(" n1 = %d\n", n);
		recvBuff[n] = 0;
		if (fputs(recvBuff, stdout) == EOF)
		{
			printf("\n Error : Fputs error\n");
		}
	}
	printf(" n2 = %d\n", n);
	printf("serv 6\n");
	if (n < 0)
	{
		printf("\n Read error \n");
	}
	//if (close(connfd) != 0)
	//{
	//	printf("error close() program\n");
	//}
	return EXIT_SUCCESS;
}
//////////////////////////////////////////////////////////////////////////////////
int client(void* id)
{
	long int counter =(long int)id+10;
	char sendBuff[1025] = {'\0'};
	time_t ticks;
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	printf( "connfd client = %d\n", connfd);
	//printf("clients threads %ld\n", (long int)id) ;

	while(counter > 0)
	{
		ticks = time(NULL);
		snprintf(sendBuff, sizeof(sendBuff), "Client [%ld]: %.24s\r\n",(long int)id, ctime(&ticks));
		printf("connfd = %d\n", connfd);
		write(connfd, sendBuff, strlen(sendBuff));
		counter--;
		sleep(1);
	}
	return EXIT_SUCCESS;
}
//////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
	pthread_t thread_server = 0;
	pthread_t threads_client[SIZE] = {0};
	int result = 0;
	long int i = 0;

	result = pthread_create(&thread_server, NULL, (void *)server, NULL);
	if (result != 0)
	{
		perror("Creating the server thread");
		return EXIT_FAILURE;
	}
	init_sock();
	for (i = 0; i < SIZE; i++)
	{
		result = pthread_create(&threads_client[i], NULL, (void *)client, (void *)i);
		//printf("number client id = %ld\n", i);
		if (result != 0)
		{
			perror("Creating the client thread");
			return EXIT_FAILURE;
		}
	}
	for (i = 0; i < SIZE; i++)
	{
		result = pthread_join(threads_client[i], NULL);
		if (result != 0)
		{
			perror("Joining the client thread");
			return EXIT_FAILURE;
		}
	}
	printf("Close client \n");
	if (close(connfd) != 0)
	{
		printf("Error close thread\n");
	}
	pthread_join(thread_server, NULL);
	if (result != 0)
	{
		perror("Joining the server thread");
		return EXIT_FAILURE;
	}
	printf(" End program\n");
	return EXIT_SUCCESS;
}
///// client socket initialization
void init_sock (void)
{
	int listenfd = 0;
	//int connfd = 0;
	int ret = 0;
	//long int counter =(long int)id+10;
	char sendBuff[1025] = {'\0'};
	time_t ticks;
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));

	//printf("clients threads %ld\n", (long int)id);

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(5000);
	bind(listenfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
	ret = listen(listenfd, 10);
	if (ret != 0)
	{
		printf("Error return function listen\n");
	}
	//printf("1\n");
	connfd = accept(listenfd, (struct sockaddr*) NULL, NULL);
	//printf("connfd = %d\n",connfd);
	if (connfd == -1)
	{
		printf("Error return function accept\n");
	}
	printf("connfd = %d\n", connfd);
}

