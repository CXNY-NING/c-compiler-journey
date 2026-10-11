#include<stdio.h>
#include<stdlib.h>
#include<errno.h>

#include"data.h"
#include"token.h"
#include"scanner.h"

int line;
int Putback;
FILE* Target_file;
const char* tokens_type[] = { "end","+","-","*","/","int" };


void init()
{
	line = 1;
	Putback = ' ';
}


int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		fprintf(stderr, "error: %s infile", argv[0]);
		exit(1);
	}

	init();

	Target_file = fopen(argv[1], "r");
	if (Target_file == NULL)
	{
		fprintf(stderr, "error: Unable to open: %s : %s", argv[1],strerror(errno));
		exit(1);
	}

	scanf_file();
	return 0;
}
