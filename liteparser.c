#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <time.h>
#include <inttypes.h>

unsigned char **hash_array;
uint64_t nooftxn;
uint8_t odd_even=0; //0 means even, 1 means odd
struct checks{
   unsigned char previous_block_header_hash[32];
   unsigned char calculated_previous_block_header_hash[32];
   unsigned char Merkle_Root[32];  
   unsigned char calculated_Merkle_Root[32];
   uint64_t total_in;//to calculate total_value of IN txn
   uint64_t total_out;//to calculate total_value of out txn   
}check;

struct IP_tx_data {
   unsigned char previous_tx_hash[32];
   uint32_t index;
   unsigned char *signature;
   unsigned char *public_key;  
   	
}IP;

uint64_t varint(FILE* BLOCK)
{
	uint8_t s1;
	uint16_t s2;
	uint32_t s3;
	uint64_t s4;
	int count=fread(&s1, 1, 1, BLOCK);//trying to get no of trancsactions
	if (count!=1)
	{
		printf("\nError reading s1");
	}

	if(s1<0xfd)
		return (uint64_t)s1;

	else if (s1==0xfd)
	{

		count=fread(&s2, 1, 2, BLOCK);//trying to get no of trancsactions
	
		if (count!=2)
		{
			printf("\nError reading s2");
		}

		return (uint64_t)s2;
	}
	else if(s1==0xfe)
	{
		count=fread(&s3, 1, 4, BLOCK);//trying to get no of trancsactions
	
		if (count!=4)
		{
			printf("\nError reading s3");
		}

		return (uint64_t)s3;
	}
	else if(s1==0xff)
	{
		count=fread(&s4, 1, 8, BLOCK);//trying to get no of trancsactions
	
		if (count!=8)
		{
			printf("\nError reading s4");
		}
		return (uint64_t)s4;
		
	}
}
	
void calculatehash(unsigned char **hash_array)
{
	unsigned char *hash_64byte = (unsigned char *)malloc(64*sizeof(unsigned char));
	int k=0;
	//uint64_t nooftxn_temp=nooftxn;
	//nooftxn_temp=nooftxn_temp/2;
	unsigned int m=0;
	while (m<nooftxn)
	{
		for(int j=0;j<64;j++)
		{
			if (j<32)
			{
				hash_64byte[j]=hash_array[m][j];
			}
			else
			{
				hash_64byte[j]=hash_array[m+1][j-32];
			}

		}
		SHA256((unsigned char *)hash_64byte,64,(unsigned char *)hash_array[k]);
		SHA256((unsigned char *)hash_array[k],32,(unsigned char *)hash_array[k]);
		k++;
		m=m+2;
	}
	free(hash_64byte);
}	

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
	
	uint32_t actual_blk_size = (uint32_t) (cal_size_of_BLOCK -  block_start_pointer); //size in field is sizeof_file - 8bytes(4 bytes magic, 4 bytes size)
	
	if (actual_blk_size!= block_size)
	{
		printf("\nBlock Size not found valid");
		printf("\nExiting...\n");
		exit(1);
	}
	
	fseek(BLOCK, block_start_pointer, SEEK_SET);          // placing the file pointer again to its previous position (8 byte from start)
	return(actual_blk_size);
		
}
void Fetch_block_header(FILE* BLOCK)
{
	uint32_t Version;
	unsigned char hashPrevBlock[32];
	unsigned char hashMerkleRoot[32];
	uint32_t Time;
	uint32_t Bits;
	uint32_t Nonce;

	printf("\n\nPrinting Block Header:- ");
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
		check.previous_block_header_hash[k]=hashPrevBlock[31-k];
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
		check.Merkle_Root[j] = hashMerkleRoot[31-j];
	}
	
	count=fread(&Time, 1, 4, BLOCK);//Reading Time of block 
	
	if (count!=4)
	{
		printf("\Error reading time stamp of block header");
	}
	time_t tx_time=Time;
	printf("\nBlock time : %s", ctime(&tx_time));
	
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


void check_previous_header(FILE* block)
{
	fseek(block, 8, SEEK_SET);
	unsigned char *Hash_1d=(unsigned char *)malloc(32*sizeof(unsigned char));
	unsigned char *Hash_2d=(unsigned char *)malloc(32*sizeof(unsigned char));
	unsigned char *block_header_content=(unsigned char*)malloc(80*sizeof(unsigned char));
	int count=fread(block_header_content, 1, 80, block);//trying to read magic number
	if (count!=80)
	{
		printf("\nCould not read the block header");
		free(Hash_1d);
		free(Hash_2d);
		free(block_header_content);
		return;
	}
	SHA256(block_header_content, 80, Hash_1d);
	SHA256(Hash_1d, 32,Hash_2d);
	for(int j=0;j<32;j++)
	{
		check.calculated_previous_block_header_hash[j]=Hash_2d[31-j];
	}
	
	free(Hash_1d);
	free(Hash_2d);
	free(block_header_content);
}

uint64_t chain_op_transactions(FILE* BLOCK)
{
	uint64_t output_script_size; 
    uint8_t *output_script;
	uint64_t value;
	uint64_t count=fread(&value, 1, 8, BLOCK);//Fetching value of transaction
	if (count!=8)
	{
		printf("\nError in getting value");
	}
	output_script_size=varint(BLOCK);//fetching ouput script size
	
	output_script = (unsigned char*)malloc(output_script_size*sizeof(unsigned char));
	
	count=fread(output_script, 1, output_script_size, BLOCK);//trying to get no of trancsactions
	if (count!=output_script_size)
	{
		printf("\nError in getting output script");
	}
	
	free(output_script);
	return value;
}

void chain_ip_transaction(FILE* BLOCK)
{
	uint8_t hash_previous_transaction[32];
	uint32_t n;
	uint64_t script_length;  
	uint8_t *input_script;
	uint32_t sequence_number;
					 
	uint64_t count=fread(&hash_previous_transaction, 1, 32, BLOCK);//Fetching Hash of previous trancsaction
	if (count!=32)
	{
		printf("\nError in getting hash_previous_transaction");
	}
	count=fread(&n, 1, 4, BLOCK);//Reading n
	if (count!=4)
	{
		printf("\nError in getting n");
	}
	
	script_length=varint(BLOCK);//calculating length of input script
	input_script=(unsigned char*)malloc(script_length*sizeof(unsigned char));
	count=fread(input_script, 1, script_length, BLOCK);//Feching input script
	if (count!=script_length)
	{
		printf("\nError in getting input script");
	}
	count=fread(&sequence_number, 1, 4, BLOCK);//Fetching sequence number
	if (count!=4)
	{
		printf("Error in getting sequence number \n");
	}

	free(input_script);	
}

void chain_Transactions(FILE* BLOCK, unsigned char *hash_to_cmp, uint32_t index, uint64_t *value)
{   
	unsigned char *Transaction_hash;
	uint32_t Transaction_version_no;
	uint64_t input_transaction_count;
	uint64_t output_transaction_count;
	uint32_t lock_time; 
	//uint64_t temp;
	int Transaction_start_pointer, Transaction_end_pointer,Transaction_size,output_tx_start;
	Transaction_start_pointer = ftell(BLOCK);
	int count=fread(&Transaction_version_no, 1, 4, BLOCK);
	if (count!=4)
	{
		printf("\nError reading Tansaction Version no.");
	}

	input_transaction_count=varint(BLOCK);

	for(unsigned int i =0;i<input_transaction_count;i++)
	{
		chain_ip_transaction(BLOCK);
	}

	output_tx_start = ftell(BLOCK);// just in case hash matches
	output_transaction_count=varint(BLOCK);
	
	for(unsigned int i =0;i<output_transaction_count;i++)
	{
		chain_op_transactions(BLOCK);
	}
	
	count=fread(&lock_time, 1, 4, BLOCK);//trying to read lock time of current transaction
	if (count!=4)
	{
		printf("\nError reading transaction locktime");
	}

	Transaction_end_pointer = ftell(BLOCK);
	Transaction_size=Transaction_end_pointer-Transaction_start_pointer;
	fseek(BLOCK, Transaction_start_pointer, SEEK_SET);
	unsigned char *Transaction_content, *Hash_1d,*Hash_2d;
	Hash_1d=(unsigned char *)malloc(32*sizeof(unsigned char));
	Hash_2d=(unsigned char *)malloc(32*sizeof(unsigned char));
	Transaction_hash=(unsigned char *)malloc(32*sizeof(unsigned char));
	Transaction_content=(unsigned char *)malloc(Transaction_size*sizeof(unsigned char));
	count=fread(Transaction_content, 1, Transaction_size, BLOCK);//trying to read lock time of current transaction
	if (count!=Transaction_size)
	{
		printf("\nError reading transaction content");
	}
	SHA256(Transaction_content, Transaction_size, Hash_1d);
	SHA256(Hash_1d,32,Hash_2d);

	int var1=0;	
	for(int i=0;i<32;i++)
	{

		if(Hash_2d[i]!=hash_to_cmp[i])
		{
			var1=1;
			break;
		}
	
	}

	if(var1==0)
	{
		//Hash match found
		//printf("\nHash match found for output tx");
		fseek(BLOCK,output_tx_start,SEEK_SET);
		output_transaction_count=varint(BLOCK);
		for(unsigned int i =0;i<output_transaction_count;i++)
		{
			if(index!=i)
				chain_op_transactions(BLOCK);
			else
			{	
				*value=chain_op_transactions(BLOCK);
				//printf("\n\n*********The Value Found properly is %llu***********\n\n",*value);
				break;
			}
		}
	
	}
	else
	{
		*value=0;
	}	
	fseek(BLOCK,Transaction_end_pointer,SEEK_SET);
	free(Transaction_content);
	free(Transaction_hash);
	free(Hash_1d);	
	free(Hash_2d);
}

void get_out_tx(unsigned char *prev_hash, uint32_t index, uint64_t *tx_value)
{

	FILE *fp;
	fp= fopen("block_chain", "rb");
	int block_seek;
	uint32_t no_of_transactions;
	int block_no=0;	
	if(fp==NULL)
	{
		printf("\n Error in opening block_chain file ");
    		exit(1);	
	}
	*tx_value=0;
	while(1)
	{
		fseek(fp, 4, SEEK_CUR);//skipped magic number read
		uint32_t blk_size = 0;
		int count=fread(&blk_size, 1, 4, fp);
		if (count!=4)
		{
			//printf("\nError reading block size");
			break; //No more block present
		}
		
		block_seek=ftell(fp);		
				fseek(fp, 80, SEEK_CUR);//skipped block header bytes
		no_of_transactions=varint(fp);
		for(uint32_t i = 0;i<no_of_transactions;i++)
		{
		
			chain_Transactions(fp,prev_hash,index,tx_value);
			if (*tx_value!=0)
				break;
		}
		if (*tx_value==0)
		{
			
			fseek(fp,block_seek,SEEK_SET);
			fseek(fp,blk_size,SEEK_CUR);
		
		}
		else
		//value found.
			break;		
		block_no++;
		//m++;
	}
	
	fclose(fp);
}
void Input_transaction(FILE* BLOCK,int loop_count)
{
	uint8_t hash_previous_transaction[32];
	uint32_t n;
	uint64_t script_length;  
	uint8_t *input_script;
	uint32_t sequence_number;
	uint64_t tx_value=0;
	uint64_t count=fread(&hash_previous_transaction, 1, 32, BLOCK);//Fetching Hash of previous trancsaction
	if (count!=32)
	{
		printf("\nError in getting hash_previous_transaction");
	}
	printf("\n\tHash of previous transaction to this input : ");
	for(int j=0;j<32;j++)
	{
		IP.previous_tx_hash[j] = hash_previous_transaction[j];	
		printf("%02x", hash_previous_transaction[31-j]);
	}		
		
	count=fread(&n, 1, 4, BLOCK);//Reading n
	if (count!=4)
	{
		printf("\nError in getting n");
	}
	printf("\n\tn : %u",n);
	IP.index=n;
	
	if(loop_count!=0)
	{
		get_out_tx(IP.previous_tx_hash, IP.index, &tx_value);
		
		if (tx_value==0)
		{	//Output for current Input not found. Hence block is invalid. To be done later
		printf("\n\nThe outputs cannot be found and hence inputs cannot be validated");
		printf("\nBlock cannot be validated without proper block_chain file");
		}
		else
			check.total_in=check.total_in+tx_value;
	}
	script_length=varint(BLOCK);			//calculating length of input script
	printf("\n\tScript length :  %"PRIu64, script_length);
	input_script=(unsigned char*)malloc(script_length*sizeof(unsigned char));
	count=fread(input_script, 1, script_length, BLOCK);//Feching input script
	if (count!=script_length)
	{
		printf("\nError in getting input script");
	}
	printf("\n\tInput script : ");
	for(uint64_t j=0;j<script_length;j++)
		printf("%02x", input_script[j]);
		printf("\n");
	
	
	count=fread(&sequence_number, 1, 4, BLOCK);//Fetching sequence number
	if (count!=4)
	{
		printf("\nError in getting sequence number");
	}
	printf("\n\tSequence No. : %x", sequence_number);
	free(input_script);	
}

void Output_transactions(FILE* BLOCK,int txnno)
{
	uint64_t output_script_size; 
	uint8_t *output_script;
	uint64_t value;
	uint64_t count=fread(&value, 1, 8, BLOCK);//Fetching value of transaction
	if (count!=8)
	{
		printf("\nError in getting value");
	}
	
	if(txnno!=0)
		check.total_out=check.total_out+value;
	printf("\n\tValue of transaction : %0.8lf LTC",(double)((value*1.0)/100000000));

	output_script_size=varint(BLOCK);//fetching ouput script size
	printf("\n\tOutput script size : %"PRIu64,output_script_size);
	output_script =(unsigned char*)malloc(output_script_size*sizeof(unsigned char));
	
	count=fread(output_script, 1, output_script_size, BLOCK);//trying to get no of trancsactions
	if (count!=output_script_size)
	{
		printf("\nError in getting output script");
	}
	printf("\n\tOutput script is : ");
	for(uint64_t j=0;j<output_script_size;j++)
		printf("%02x", output_script[j]);
	free(output_script);
}
void Transactions(FILE* BLOCK,int txno)
{   
	unsigned char *Transaction_hash;
	uint32_t Transaction_version_no;
	uint64_t input_transaction_count;
	uint64_t output_transaction_count;
	uint32_t lock_time;
	int Transaction_start_pointer, Transaction_end_pointer,Transaction_size;
	Transaction_start_pointer = ftell(BLOCK);
		
	int count=fread(&Transaction_version_no, 1, 4, BLOCK);//reading transaction version no.
	if (count!=4)
	{
		printf("\nError reading Tansaction Version no.");
	}

	printf("\nVersion of Transaction : %u",Transaction_version_no);  
  	input_transaction_count=varint(BLOCK);

	printf("\nNo of input in current transaction : %"PRIu64, input_transaction_count);
	for(unsigned int i =0;i<input_transaction_count;i++)
	{
		printf("\nvin %d",i+1);
		printf("\n{");
		Input_transaction(BLOCK,txno);
		printf("\n}");
	}
	
	output_transaction_count=varint(BLOCK);
	
	printf("\nNo of output from current transaction : %"PRIu64, output_transaction_count);
	for(unsigned int i =0;i<output_transaction_count;i++)
	{
		printf("\nvout %d",i+1);
		printf("\n{");
		Output_transactions(BLOCK,txno);
		printf("\n}");
	}
	
	count=fread(&lock_time, 1, 4, BLOCK);//trying to read lock time of current transaction
	if (count!=4)
	{
		printf("\nError reading transaction locktime");
	}

	Transaction_end_pointer = ftell(BLOCK);
	Transaction_size=Transaction_end_pointer-Transaction_start_pointer;
	//printf("\nCurrent transaction size is= %d\n",Transaction_size);
	fseek(BLOCK, Transaction_start_pointer, SEEK_SET);
	unsigned char *Transaction_content, *Hash_1d,*Hash_2d;
	Hash_1d=(unsigned char *)malloc(32*sizeof(unsigned char));
	Hash_2d=(unsigned char *)malloc(32*sizeof(unsigned char));
	Transaction_hash=(unsigned char *)malloc(32*sizeof(unsigned char));
	Transaction_content=(unsigned char *)malloc(Transaction_size*sizeof(unsigned char));
	count=fread(Transaction_content, 1, Transaction_size, BLOCK);//trying to read lock time of current transaction
	if (count!=Transaction_size)
	{
		printf("\nError reading transaction content");
	}
	SHA256(Transaction_content, Transaction_size, Hash_1d);
	SHA256(Hash_1d, 32,Hash_2d);
	printf("\n\nHash (TXID) of this transaction is: ");
	for(int j=0;j<32;j++)
	{
		Transaction_hash[j]=Hash_2d[31-j];
		printf("%02x", Hash_2d[31-j]);
		hash_array[txno][j]=Hash_2d[j]; // Hash copied in array for merkel root calculation
	}
	
	free(Transaction_content);
	free(Transaction_hash);
	free(Hash_1d);	
	free(Hash_2d);
}

void validate()
{
	int var1=0;
	int var2=0;

	printf("\n\nPrev Hash validation");
	printf("\n--------------------");
	for(int i=0;i<32;i++)
	{

		if(check.calculated_previous_block_header_hash[i]!=check.previous_block_header_hash[i])
		{
			var1=1;
			printf("\nPrev block Hash not matching!");
			break;
		}
	
	}

	if(var1==0)
		printf("\nPrev block Hash matched successfully!");
	
	printf("\n\nMerkel Root Hash Validation");
	printf("\n---------------------------");
	for(int i=0;i<32;i++)
	{

		if(check.calculated_Merkle_Root[i]!=check.Merkle_Root[i])
		{
			var2=1;
			printf("\nMerkle root Hash not matching!\n");
			break;
		}
	
	}
	if(var2==0)
		printf("\nMerkle root Hash matched successfully!\n");

}

int main(int argc, char **argv)
{
	FILE *BLOCK;
	FILE *prev_block;
	uint64_t no_of_transactions;
	int retval=0;
	if(argc==2)
	{
		BLOCK = fopen(argv[1], "rb");
		if(BLOCK==NULL)
		{
			printf("\nError in opening block file ");
		}
	}
	else
	{
		printf("\nUSAGE: ./liteparser <blockfile>");
		exit(0);
	}
	prev_block = fopen("block_prev", "rb");
	if(prev_block==NULL)
	{
		printf("\nError in opening block file ");
		exit(1);	
	}
	retval=Magic_number(BLOCK);
	if (retval==0)
	{
		printf("\nMagic Number matched successfully ");
	}
	else
	{
		printf("\nMagic Number invalid for Litecoin ");
		printf("\nExiting...\n");
		exit(1);
	}
			   
	retval=calculate_block_size(BLOCK);
	if (retval!=0)
	{
		printf("\nBlock Size found valid");
	}
	
	//Parsing the rest of the block
	Fetch_block_header(BLOCK);
	no_of_transactions=varint(BLOCK);
	printf("\nNo. of transactions in this block : %"PRIu64,no_of_transactions);
	check_previous_header(prev_block);
	
	nooftxn=(uint64_t)no_of_transactions;
	
	if (nooftxn % 2 == 0)
		odd_even=0;
	else
	{	odd_even=1;
		nooftxn++;
	}
	
	hash_array = (unsigned char **)malloc(sizeof(unsigned char *)*nooftxn);
	for (uint32_t i=0;i<nooftxn;i++)
	{
		hash_array[i]=(unsigned char *)malloc(sizeof(unsigned char)*32);
	}
	
	check.total_out=0;
	check.total_in=0;
	//printf("\nNo of transaction in this block =%llu\n\n",no_of_transactions);
	for(uint64_t i = 0;i<no_of_transactions;i++)
	{
		printf("\n\n*****Transaction %"PRIu64"*****",i+1);
		Transactions(BLOCK,i);
	}
	//uint64_t fee;
	printf("\n\n*****BLOCK VALIDATION CHECKS*****");
	printf("\n\nTransaction validation");
	printf("\n----------------------");
	//printf("\nTotal Value out : %0.8lf LTC", check.total_out);
	//printf("\nTotal Value in  : %llu", check.total_in);

	if (check.total_out > check.total_in)
	{
		printf("\n\nThe transactions are invalid as total-out is more than total-in");
		printf("\nThe block is invalid");
	} 
	else
	{
		printf("\nThe total value in > total value out");
		printf("\nThe transactions are valid");
		printf("\nThe total_in = %0.8lf LTC",(double)((check.total_in*1.0)/100000000));
		printf("\nThe total_out = %0.8lf LTC",(double)((check.total_out*1.0)/100000000));
		printf("\nThe miner's fee = %0.8lf LTC", (double)(((check.total_in-check.total_out)*1.0)/100000000));
	}
	
	//Merkel Root calculation
	if (odd_even==1)
		for(int i=0;i<32;i++)
			hash_array[nooftxn-1][i]=hash_array[nooftxn-2][i]; //copy last hash again to make even
	//find merkle root
	
	while(nooftxn!=1)
	{
		calculatehash(hash_array);
		nooftxn=nooftxn/2;
		
		if (nooftxn==1)
			break;
	
		if(nooftxn%2!=0)
		{	
			nooftxn=nooftxn+1;
			for(int i=0;i<32;i++)
				hash_array[nooftxn-1][i]=hash_array[nooftxn-2][i]; //copy last hash again to make even
		}
	}
		
	
	for(int j=0;j<32;j++)
	{		
		//printf("%02x", hash_array[nooftxn-1][31-j]);
		check.calculated_Merkle_Root[j]=hash_array[nooftxn-1][31-j];
	}
	
	printf("\n");
	validate();//check whether previous block header hash matches

	fclose(BLOCK);
	fclose(prev_block);
	if (no_of_transactions % 2 == 0)
	{
		for(uint64_t i=0;i<no_of_transactions;i++)	
			free(hash_array[i]);
	}
	else
	{
		for(uint64_t i=0;i<no_of_transactions+1;i++)	
			free(hash_array[i]);
	}	
	free(hash_array);
	return 0;

}

