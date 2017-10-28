#ifndef Block_h
#define Block_h

#include <stdint.h>

typedef struct {
    uint8_t hash_previous_transaction[32];
    uint32_t Transaction_index;
    uint32_t script_length;  
    uint8_t *input_script;
    uint32_t sequence_number;
} Transaction_in;

typedef struct {
    uint64_t value; 
    uint32_t output_script_size; 
    uint8_t *output_script;
} Transaction_out;

typedef struct {

    uint32_t Transaction_version_no;
    uint32_t input_count;
    uint32_t timestamp;
    Transaction_in *tx_input;
    uint32_t tx_output_count;  
    Transaction_out *tx_output;
    uint32_t lock_time;
    uint8_t Transaction_hash[32];		
    uint32_t Transaction_length;			
    uint32_t blk_file_index;					
    uint32_t file_Offset;					
    uint32_t Transaction_ID;	
} Transaction_t;

typedef struct {
    uint32_t Magic_no;
    uint32_t Block_size;
    uint32_t Version;
    uint8_t hash_previous_block[32];
    uint8_t hash_Merkle_root[32];
    uint32_t time_stamp;
    uint32_t current_target_bits;
    uint32_t Nonce;
    uint64_t Transaction_counter; 
    Transaction_t *Transanction;
    
} Block_Structure;

#endif /* BLOCK_H */
