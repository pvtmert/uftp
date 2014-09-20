#include "tcpip.h"

void vdebug(char *fmt, va_list args)
{
	char *buffer = malloc(BUFSIZE);
	int bytes = sprintf(buffer,DEBUG_PREFIX"%s",PADDING,time(NULL),fmt);
	vsprintf(buffer,buffer,args);
	fprintf(stderr,"%s\r\n",buffer);
	return;
}

void debug(char *str, ...)
{
	#ifndef DEBUG
	  return;
	#endif
	va_list args;
	va_start(args,str);
	vdebug(str,args);
	va_end(args);
	return;
}

int main(int argc, char *argv[])
{
	debug("application started...");
	char *buf = malloc(BUFSIZE);
	pid_t ppid, mainpid = getpid();
	if(argc < 1)
	{
		printf("usage: [port] [child-count] \n");
		return 1;
	}
	char port[6];
	sprintf(port,"%d",(argc < 2)?PORT:atoi(argv[1]));
	int sockfd, bindfd, accfd, listenfd, retval;
	struct addrinfo prfx, *us;
	memset(&prfx,0,sizeof(prfx));
	prfx.ai_family = AF_UNSPEC;
	prfx.ai_socktype = SOCK_STREAM;
	prfx.ai_flags = AI_PASSIVE;
	retval = getaddrinfo(NULL,port,&prfx,&us);
	if(retval < 0)
		perror("[fail] getaddrinfo");
	sockfd = socket(us->ai_family,us->ai_socktype,us->ai_protocol);
	if(sockfd < 0)
		perror("[fail] socket");
	bindfd = bind(sockfd,us->ai_addr,us->ai_addrlen);
	if(bindfd < 0)
		perror("[fail] bind");
	listenfd = listen(sockfd,(argc < 3)?MAX_CONNS:atoi(argv[2]));
	if(listenfd < 0)
		perror("[fail] listen");
	struct sockaddr_storage guest;
	socklen_t addr_size = sizeof(guest);
	bool ischild = false;
	debug("in loop");
	char *cliname = malloc(BUFSIZE);
	while(!ischild)
	{
		accfd = accept(sockfd,(struct sockaddr*)&guest,&addr_size);
		if(accfd < 0)
			perror("[fail] accept");
		if(!ischild)
			fork();
		else
			break;
		if((ppid = getpid()) == mainpid)
			continue;
		debug("forked... pid: %d",ppid);
		retval = recv(accfd,buf,BUFSIZE,0);
		if(retval < 0)
		{
			perror(NULL);
			break;
		}
		//buf[retval] = NULL;
		if(strncmp(buf,HELO_PROT,strlen(HELO_PROT)))
			break;
		strcpy(cliname,strtok(buf+strlen(HELO_PROT),"\r\n"));
		printf("%s is connected...\n",cliname);
		sprintf(buf,SRV_HEADER"%u\n",time(NULL)/10);
		retval = send(accfd,buf,strlen(buf),0);
		FILE *fp;
		writer:
			retval = recv(accfd,buf,BUFSIZE,0);
			if(retval < 1 || buf == NULL || buf[0] == 28)
				break;
			//for(int i=retval;i<BUFSIZE;i++) buf[i] = NULL;
			if(!strncmp(buf,TIME_DIFF,strlen(TIME_DIFF)))
			{
				printf("[fail] client time is not in sync... (%s)\n",
					strtok(buf+strlen(TIME_DIFF),"\r\n"));
				break;
			}
			if(strncmp(buf,CLI_HEADER,strlen(CLI_HEADER)))
				break;
			char *ptr = strtok(buf+strlen(CLI_HEADER),"\r\n");
			printf("file switched: %s\n",ptr);
			fp = fopen(ptr,"wb");
			//free(ptr);
			retval = 1;
			while(retval > 0)
			{
				char r;
				retval = recv(accfd,&r,1,0);
				printf("%d ",r);
				if(r == 28 || retval < 1 || r < 0)
					break;
				fwrite(&r,sizeof(void),1,fp);
			}
			fclose(fp);
			printf("\n");
			goto writer;
	}
	printf("Client %s is disconnected, ending PID: %d\n",cliname,ppid);
	free(cliname);
	sprintf(buf,TIME_DIFF"GOODBYE\n");
	retval = send(accfd,buf,strlen(buf),0);
	retval = close(accfd);
	debug("conn closed");
	if(retval < 0)
		perror("[fail] close");
	close(sockfd);
	return 0;
}
