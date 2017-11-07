#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int Magic_number(FILE* BLOCK)
{
	unsigned char magic[5];
	int count=fread(&magic, 1, 4, BLOCK);//trying to read magic number
	if (count!=4)
	{
		printf("Error reading magic number\n");
	}
	printf("Magic no read as:");
	for(int i=0;i<4;i++)
	{	
		printf("%2x",magic[i]);
	}
	magic[4]='\0';
	printf("\n");
	
	if (strcmp(magic,"fbc0b6db")!=0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int main()
{
	FILE *BLOCK;
	int retval=0;
	BLOCK = fopen("block_current", "rb");
	if(BLOCK==NULL)
	{
		printf("\n Error in opening block file ");
      	    			
	}
	retval=Magic_number(BLOCK);
	if (retval==0)
	{
		printf("\n Magic Number matched successfully ");
	}
	else
	{
		printf("\n Magic Number invalid for Litecoin ");
	}
	return 0;
}
