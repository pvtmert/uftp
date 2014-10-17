#include "tcpip.h"

// debug output with unix time in front of text...
// appends newline to end of the output
void vdebug(char *fmt, va_list args)
{
	FILE *out = stderr;
	char *buffer = malloc(BUFSIZE);
	int bytes = sprintf(buffer,DEBUG_PREFIX"%s",PADDING,(unsigned)time(NULL),fmt);
	buffer[bytes] = NULL;
	vfprintf(out,buffer,args);
	fprintf(out,"\r\n");
	return;
}

// takes input to forward text to vdebug function,
// vdebug takes va_list _args_
void debug(char *str, ...)
{
	#ifndef DEBUG
	  return;
	#endif
	// initialize variable arguments list
	va_list args;
	// define starting point of 'list'
	va_start(args,str);
	// send list to va_list function
	vdebug(str,args);
	// clear ptr by freeing
	va_end(args);
	return;
}

void usage()
{
		fprintf(stderr,"usage: [port] [child-count] \n");
		exit(1);
}

// the main function
int main(int argc, char *argv[])
{
	// if there is no arguments, which looks impossible...
	// OR there is invalid port number...
	if(argc > 1)
		if(!atoi(argv[1]))
			usage();
	// notify the user or log it
	debug("application started...");
	// our general recieve and send buffer
	char *buf = malloc(BUFSIZE);
	// store main pid in memory so we can check
	// if we are children of main thread
	pid_t ppid, mainpid = getpid();
	// the port's buffer for storing listener,
	// 65536 is the maximum possible value and
	// length is 5, with null it is 6.
	char port[6];
	// parse the port number according to argument count,
	// if there is no argument than program itself,
	// then assign default port which is defined in "tcpip.h"
	// else parse the port number from argument #2 with the
	// mod of 65536 because it cant exceed that number.
	sprintf(port,"%d",(argc < 2)?PORT:atoi(argv[1])%65536);
	// definition of file descriptors, retval is used for
	// check if it returned error (send recv or socket)
	int sockfd, bindfd, accfd, listenfd, retval;
	// standard structs for connecting server through,
	// the prfx means prefix for assigning base values to
	// socket connection like AF_INET or AF_INET6
	// us is pointer to our data -us-
	struct addrinfo prfx, *us;
	// fill memory of prefix by zeros so it wont overlap with
	// actual ptr.
	memset(&prfx,0,sizeof(prfx));
	// set the details of connection, acual listener...
	prfx.ai_family = AF_UNSPEC;
	prfx.ai_socktype = SOCK_STREAM;
	prfx.ai_flags = AI_PASSIVE;
	// return and fill our ptr by using port and prefixes
	// from above
	retval = getaddrinfo(NULL,port,&prfx,&us);
	if(retval < 0)
		perror("[fail] getaddrinfo");
	// open socket using filled values
	sockfd = socket(us->ai_family,us->ai_socktype,us->ai_protocol);
	if(sockfd < 0)
		perror("[fail] socket");
	// bind to socket so it will return it is used by an application
	bindfd = bind(sockfd,us->ai_addr,us->ai_addrlen);
	if(bindfd < 0)
		perror("[fail] bind");
	// open listener, with the max number of connections from depending
	// on command-line arguments or default value depending on which is
	// possible...
	listenfd = listen(sockfd,(argc < 3)?MAX_CONNS:atoi(argv[2]));
	if(listenfd < 0)
		perror("[fail] listen");
	// the addressinfo of out client... which is named 'guest'
	struct sockaddr_storage guest;
	// we need sizeof guest so we wont overflow or move ahead of it
	// also need to return pointer etc.
	socklen_t addr_size = sizeof(guest);
	bool ischild = false;
	debug("in loop");
	char *cliname = malloc(BUFSIZE);
	prg_space:
	while(!ischild)
	{
		accfd = accept(sockfd,(struct sockaddr*)&guest,&addr_size);
		if(accfd < 0)
			perror("[fail] accept");
		if(!ischild)
			fork();
		else
			break;
		//ischild = true;
		if((ppid = getpid()) == mainpid)
			continue;
		debug("forked... pid: %d",ppid);
		retval = recv(accfd,buf,BUFSIZE,0);
		if(retval < 0)
		{
			perror(NULL);
			break;
		}
		buf[retval] = NULL;
		if(strncmp(buf,HELO_PROT,strlen(HELO_PROT)))
			break;
		strcpy(cliname,strtok(buf+strlen(HELO_PROT),"\r\n"));
		printf("%s is connected...\n",cliname);
		sprintf(buf,SRV_HEADER"%u\n",(unsigned)time(NULL)/10);
		retval = send(accfd,buf,strlen(buf),0);
		FILE *fp;
		writer:
			retval = recv(accfd,buf,BUFSIZE,0);
			if(retval < 1 || (buf[0] == endchar && buf[1] == EOF))
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
				retval = recv(accfd,&r,sizeof(char),0);
				//printf("%d ",r);
				if(retval < 0 || r == endchar)
				{
					char n;
					retval = recv(accfd,&n,sizeof(char),0);
					if(n != endchar || n == EOF)
						break;
					fwrite(&r,sizeof(char),1,fp);
					fwrite(&n,sizeof(char),1,fp);
					continue;
				}
				fwrite(&r,sizeof(char),1,fp);
			}
			fclose(fp);
			//printf("\n");
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
