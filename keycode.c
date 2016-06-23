#include <stdio.h>

typedef enum { false, true } bool;

int main(int argc, char *argv[])
{
	if(argc > 1)
	{
		for(int i=2;i<argc;i++)
		{
			int j=0;
			while(argv[i][j] != NULL)
				printf("% 3u ",(unsigned char)argv[i][j++]);
			printf("\r\n");
		}
		return 0;
	}
	int ln = 1;
	while(true)
	{
		char c = fgetc(stdin);
		if(feof(stdin))
			break;
		printf("% 3u ",(unsigned char)c);
		if(ln%18 == 0)
		{
			ln = 0;
			printf("\r\n");
		}
		ln += 1;

	}
	printf("\r\n");
	return 0;
}
