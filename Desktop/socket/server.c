#include<stdio.h>

int main(int argc, char**argv){
	int i = 0;
	printf("argc : %d    \n*****argv*****\n", argc);
	while (*(argv + 1))
	{
		puts(*(argv + (i + 1)));
	}
	return 0;
}