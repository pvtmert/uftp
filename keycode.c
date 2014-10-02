#include <stdio.h>

typedef enum { false, true } bool;

int main(int argc, char **argv)
{
	if(argc > 1)
	{
		for(int i=2;i<argc;i++)
		{
			int j=0;
			while(argv[i][j] != NULL)
				printf("%d ",argv[i][j++]);
			printf("\r\n");
		}
		return 0;
	}
	while(true)
	{
		char c = fgetc(stdin);
		printf("%d ",c);
		if(c < 0)
			break;
	}
	return 0;
}
