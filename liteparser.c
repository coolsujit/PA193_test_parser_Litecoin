#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <time.h>

int Magic_number(FILE* BLOCK)
{
	unsigned char magic_check[4] = {0xfb,0xc0,0xb6,0xdb};
	unsigned char magic[4];
	int verify=0;
	int count = fread(&magic, 1, 4, BLOCK);//trying to read magic number
	if (count!=4)
	{
		printf("\nError reading magic number");
	}
	printf("\nMagic no read as:");
	for(int i=0;i<4;i++)
	{	
		printf("%2x",magic[i]);
	}
	for(int i=0;i<4;i++)
	{	
		if (magic[i]!=magic_check[i])
		{
			verify=1;
			break;
		}
	}
	if (verify!=0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int calculate_block_size(FILE* BLOCK)
{
	uint32_t block_size = 0;
	int count=fread(&block_size, 1, 4, BLOCK);
	if (count!=4)
	{
		printf("\nError reading block size");
	}
	
	printf("\nBlock_size is : %u",block_size);

	/*To calculate block size*/
	
  	uint32_t block_start_pointer = 0;
	block_start_pointer = ftell(BLOCK);  //where is file pointer righ now save it 
	fseek(BLOCK, 0, SEEK_END);          // placing the file pointer to end of file
	int cal_size_of_BLOCK = ftell(BLOCK);      // determining the size of plain text file
	
	int actual_blk_size = cal_size_of_BLOCK -  block_start_pointer ; //size in field is sizeof_file - 8bytes(4 bytes magic, 4 bytes size)
	
	if (actual_blk_size!= block_size)
	{
		printf("\nIncorrect block size");
		return 1;
	}
	
	fseek(BLOCK, block_start_pointer, SEEK_SET);          // placing the file pointer again to its previous position (8 byte from start)
	return 0;
	
}

void Fetch_block_header(FILE* BLOCK)
{
	uint32_t Version;
	unsigned char hashPrevBlock[32];
	unsigned char hashMerkleRoot[32];
	uint32_t Time;
	uint32_t Bits;
	uint32_t Nonce;

	printf("\nPrinting Block Header:- \n");
	int count=fread(&Version, 1, 4, BLOCK);//Reading version from block file
	if (count!=4)
	{
		printf("\nError in reading version number");
	}
	printf("\nVersion : %u", Version);
	
 	count=fread(&hashPrevBlock, 1, 32, BLOCK);//Reading Hash of previous block 
	
	if (count!=32)
	{
		printf("\nError reading previous block header hash");
	}
	printf("\nHash of previous block : ");
	
	for(int k=0;k<32;k++)
	{
		printf("%02x",hashPrevBlock[31-k]);
	}
	
	count=fread(&hashMerkleRoot, 1, 32, BLOCK);//Reading Hash of merkle root of the block 
	if (count!=32)
	{
		printf("\nError reading merkle root hash");
	}
	
	printf("\nHash of Merkle Root : ");

	for(int j=0;j<32;j++)
	{
		printf("%02x", hashMerkleRoot[31-j]);
	}
	count=fread(&Time, 1, 4, BLOCK);//Reading Time of block 
	if (count!=4)
	{
		printf("error reading time stamp of block header \n");
	}
	printf("\nBlock time : %u", Time);
	count=fread(&Bits, 1, 4, BLOCK);//Reading bits (difficulty level) of block 
	
	if (count!=4)
	{
		printf("\nError reading Bits of  block header");
	}
	printf("\nBlock bits : %2x", Bits);

	count=fread(&Nonce, 1, 4, BLOCK);//Reading Nonce from block header
	
	if (count!=4)
	{
		printf("\nError reading Nonce block header");
	}
	printf("\nNonce : %u", Nonce);	
}

int main()
{
	FILE *BLOCK;
	int retval=0;
	BLOCK = fopen("block_current", "rb");
	if(BLOCK==NULL)
	{
		printf("\nError in opening block file ");
      	    			
	}
	retval=Magic_number(BLOCK);
	if (retval==0)
	{
		printf("\nMagic Number matched successfully ");
	}
	else
	{
		printf("\nMagic Number invalid for Litecoin ");
	}
	
	retval=calculate_block_size(BLOCK);
	if (retval==0)
	{
		printf("\nBlock Size found valid");
	}
	else
	{
		printf("\nBlock Size not found valid");
	}
	//Parsing the rest of the block
	Fetch_block_header(BLOCK);
	
	return 0;
}
