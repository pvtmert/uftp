#include "tcpip.h"

int main(int argc, char *argv[])
{
	if(argc < 3)
	{
		printf("usage: <host> <port> [FILE(S)...] \n");
		return 1;
	}
	char port[6];
	sprintf(port,"%d",(argc < 3)?PORT:atoi(argv[2]));
	int retval, sockfd, connfd; //bindfd;
	struct addrinfo prfx, *srv;
	memset(&prfx,0,sizeof(prfx));
	prfx.ai_family = AF_UNSPEC;
	prfx.ai_socktype = SOCK_STREAM;
	retval = getaddrinfo(argv[1],port,&prfx,&srv);
	if(retval < 0)
		perror("[fail] getaddrinfo");
	sockfd = socket(srv->ai_family,srv->ai_socktype,srv->ai_protocol);
	if(sockfd < 0)
		perror("[fail] socket");
	/*
	bindfd = bind(sockfd,srv->ai_addr,srv->ai_addrlen);
	if(bindfd < 0)
		perror("[fail] bind");
	*/
	connfd = connect(sockfd,srv->ai_addr,srv->ai_addrlen);
	if(connfd < 0)
		perror("[fail] connect");
	char *buf = malloc(BUFSIZE);
	gethostname(buf,BUFSIZE);
	char *backup = malloc(BUFSIZE);
	sprintf(backup,HELO_PROT"%s\n",buf);
	retval = send(sockfd,backup,strlen(backup),0);
	free(backup);
	retval = recv(sockfd,buf,BUFSIZE,0);
	char *pos = buf + strlen(SRV_HEADER);
	unsigned server_time = atoi(pos);
	unsigned local_time = time(NULL)/10;
	if(local_time != server_time && false)
	{
		sprintf(buf,TIME_DIFF"%u\n",time(NULL)/10);

		send(sockfd,buf,strlen(buf),0);
		free(buf);
		close(connfd);
		close(sockfd);
		printf("Error: Your clock is not in sync with server!\n"
				"Difference (s-c): %d\n",server_time-local_time);
		return 5;

	}
	FILE *fp;
	for(int i=3;i<argc;i++)
	{
		if(!strcmp(argv[i],"-"))
		{
			fp = stdin;
			sprintf(buf,CLI_HEADER"%u.txt\n",(unsigned)time(NULL));
		}else{
			fp = fopen(argv[i],"rb");
			sprintf(buf,CLI_HEADER"%s\n",argv[i]);
		}
		if(fp == NULL)
			continue;
		printf("Sending file: %s\n",argv[i]);
		retval = send(sockfd,buf,BUFSIZE,0);
		if(retval < 0)
			perror(NULL);
		while(!feof(fp))
		{
			char b;
			fread(&b,sizeof(void),1,fp); // was void
			if(feof(fp) || false && (argv[i] == '-' && (b == NULL || b == EOT || b < 0)))
				break;
			if(b == endchar)
				send(sockfd,&endchar,sizeof(char),0);
			send(sockfd,&b,sizeof(char),0);
		}
		retval = send(sockfd,&endchar,sizeof(char),0);
		char c = EOF;
		retval = send(sockfd,&c,sizeof(char),0);
		fclose(fp);
	}
	//
	char connfinish[] = {endchar,NULL};
	retval = send(sockfd,connfinish,BUFSIZE,0);
	if(retval < 0)
		perror("[send]");
	retval = recv(sockfd,buf,BUFSIZE,0);
	if(retval < 0)
		perror(NULL);
	buf[retval] = NULL;
	printf("Server: %s\n",strtok(buf+strlen(TIME_DIFF),"\r\n"));
	free(buf);
	close(connfd);
	close(sockfd);
	return 0;
}
