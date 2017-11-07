#include<stdio.h>
#include<stdlib.h>
#include<string.h>
void Magic_number(FILE* BLOCK)
{
	unsigned char magic[5];
	int count=fread(&magic, 1, 4, BLOCK);//trying to read magic number
	if (count!=4)
		{
			printf("error reading magic number\n");
		}
	printf("Magic no read as:");
	for(int i=0;i<4;i++)
		{	
		printf("%2x",magic[i]);
		}
	magic[4]='\0';
	printf("\n");
	
	if (strcmp(magic,"fbc0b6db")==0)
		{
		printf("Incorrect Magic number\n");
		}
}

int main()
{
	FILE *BLOCK;
	BLOCK = fopen("parser_s", "rb");
		if(BLOCK==NULL)
		{
			printf("\n Error in opening block file ");
      	    			
		}
	Magic_number(BLOCK);
	
	return 0;


}
