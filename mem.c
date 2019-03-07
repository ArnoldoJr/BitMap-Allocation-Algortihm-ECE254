/**
 * @file memory.c
 * @brief: ECE254 Lab: memory allocation algorithm comparison template file
 * @author: Arnoldo Rodriguez
 * @date: 2018-11-26
 */

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include "mem.h"

/* defines */

/* global variables */
void *bf_mem_block;
void *wf_mem_block;

/* Functions */
size_t get_array_size(char* mem_block);
size_t get_first_data_elem();
size_t get_block_count();
void output_values(int in, int ext_frag);

/*************************************************************** 
 || 					Memory initializers 		   	 	  ||
 ***************************************************************/
int best_fit_memory_init(size_t size) {
	size_t control_size = 0;
	int i;
	/* Return -1 if size is too small
	 * We need at least one block + overhead */
	if (size < 14){
		return -1;
	}
	bf_mem_block = malloc(sizeof(char) * size);

	// Saving array size in the first 8 bytes
	for(i = 0; i < 8; ++i){
		((char*)bf_mem_block)[i] = size >> 8*i;
	}

	// Initializing control structure:
	control_size = (size - 8) / 34;
	if( (size - 8) % 34 >= 6){
		++control_size;
	}
	for (i = 0; i < control_size*2; i++)  {
		((char*)bf_mem_block)[i + 8] = 0;
	}
	return 0;
}

int worst_fit_memory_init(size_t size) {
	size_t control_size = 0;
	int i;
	/* Return -1 if size is too small
	 * We need at least one block(4) + overhead(8+2) */
	if (size < 14){
		return -1;
	}
	wf_mem_block = malloc(sizeof(char) * size);

	for(i = 0; i < 8; ++i){
		((char*)wf_mem_block)[i] = 0;
	}
	// Saving array size in the first 8 bytes
	for(i = 0; i < 8; ++i){
		((char*)wf_mem_block)[i] = size >> 8*i;
	}

	// Initializing to zero the control structure:
	control_size = (size - 8) / 34;
	if( (size - 8) % 34 >= 6){
		++control_size;
	}
	for (i = 0; i < control_size*2; i++)  {
		((char*)wf_mem_block)[i + 8] = 0;
	}
	return 0;
}

/*************************************************************** 
 * 						Memory allocators 					   *
 ***************************************************************/
void *best_fit_alloc(size_t size) {
	if(size == 0)
		return NULL;
	
	size_t data_index = get_first_data_elem(bf_mem_block); 								// index of the first data value.
	size_t block_count = get_block_count(bf_mem_block);									// number of mem blocks.
	size_t blk_size_index = (block_count%8)?(block_count/8 + 9):(block_count/8 + 8);	// start of mem-pool-size control data

	// printf("Array size: %ld\n", get_array_size(bf_mem_block));
	// printf("data index: %ld\n", data_index);
	// printf("Block size index: %ld\n", blk_size_index);
	// printf("block count: %ld\n", block_count);

	int i;					// bit-counter
	size_t j = 0,	 		// current bit in the control data.
		   index = 8, 		// Array index = overhead + 0
		   temp_count = 0,
		   count = -1,
		   loc = -1,
		   temp_loc = 0;

	char temp_byte = 0;

	while (index < blk_size_index){

		temp_byte = ((char*)bf_mem_block)[index];

		// Looping through all bits of the control data.
		for (i = 0; i < 8; ++i){

			if (j >= block_count){
				// No more blocks in the array;
				break;
			}

			if ((temp_byte & 1) == 0){
				// We found a free block.
				if (temp_count == 0){
					temp_loc = j;
				}
				temp_count += 4;
			} else {
				// The block is occupied.
				if (count != -1 && temp_count < count && temp_count >= size) {
					// We found a smaller free block.
					count = temp_count;
					loc = temp_loc;
				} else if (temp_count >= size && count == -1) {
					// This is the first free block we find.
					count = temp_count;
					loc = temp_loc;
				}
				temp_count = 0;
			}
			// moving to next blockads
			temp_byte >>= 1;
			++j;
		}

		// The last bit was free
		if (j >= block_count) {
			if (count != -1 && temp_count < count && temp_count >= size) {
				// We found a smaller free block.
				count = temp_count;
				loc = temp_loc;
			} else if (temp_count >= size && count == -1) {
				// This is the first free block we find.
				count = temp_count;
				loc = temp_loc;
			}
			break;
		}
		++index;
	}

	if (loc != -1) {
		// We found the desired block! Marking bits as occupied
		size_t g,
			   s = ((loc + 1) / 8) + 7;
		int mask = 1,
			h = 0;

		if ((loc + 1) % 8) {
			++s;
		}
		h = loc % 8;
		mask <<= (loc % 8);
		// printf("Loc: %ld\n", loc);
		// printf("S: %ld\n", s);
		// Marking the start of the sub-memory pool - writing 1 to it
		((char*)bf_mem_block)[ blk_size_index + s - 8 ] = (int)((char*)bf_mem_block)[ blk_size_index + s - 8 ] | mask;

		size_t occup_blks = (size%4)?(size/4 + 1):size/4;

		for (g = 0; g < occup_blks; ++g){
			if (h == 8){
				++s;
				h = 0;
				mask = 1;
			}
			((char*)bf_mem_block)[s] = ((char*)bf_mem_block)[s] | mask;
			++h;
			mask <<= 1;
		}
		// Return location
		return &(bf_mem_block[data_index + loc*4]);

	} else {
		// We could not find the requested free space.
		return NULL;
	}
	return NULL;
}


void *worst_fit_alloc(size_t size) {
	if(size == 0)
		return NULL;

	size_t data_index = get_first_data_elem(wf_mem_block); 								// index of the first data value.
	size_t block_count = get_block_count(wf_mem_block);									// number of mem blocks.
	size_t blk_size_index = (block_count%8)?(block_count/8 + 9):(block_count/8 + 8);	// start of mem-pool-size control data

	int i;					// bit-counter
	size_t j = 0,	 		// current bit in the control data.
		   index = 8, 		// Array index = overhead + 0
		   temp_count = 0,
		   count = -1,
		   loc = -1,
		   temp_loc = 0;

	char temp_byte = 0;
	while (index < blk_size_index){

		temp_byte = ((char*)wf_mem_block)[index];

		// Looping through all bits of the control data.
		for (i = 0; i < 8; ++i){

			if (j >= block_count){
				// No more blocks in the array;
				break;
			}

			if ((temp_byte & 1) == 0){
				// We found a free block.
				if (temp_count == 0){
					temp_loc = j;
				}
				temp_count += 4;
			} else {
				// The block is occupied.
				if (count != -1 && temp_count > count && temp_count >= size) {
					// We found a bigger free block.
					count = temp_count;
					loc = temp_loc;
				} else if (temp_count >= size && count == -1) {
					// This is the first free block we find.
					count = temp_count;
					loc = temp_loc;
				}
				temp_count = 0;
			}
			// moving to next blockads
			temp_byte >>= 1;
			++j;
		}

		// The last bit was free
		if (j >= block_count) {
			if (count != -1 && temp_count > count && temp_count >= size) {
				// We found a bigger free block.
				count = temp_count;
				loc = temp_loc;
			} else if (temp_count >= size && count == -1) {
				// This is the first free block we find.
				count = temp_count;
				loc = temp_loc;
			}
			break;
		}
		++index;
	}

	if (loc != -1) {
		// We found the desired block! Marking bits as occupied
		size_t g,
			   s = ((loc + 1) / 8) + 7;
		int mask = 1,
			h = 0;

		if ((loc + 1) % 8) {
			++s;
		}
		h = loc % 8;
		mask <<= (loc % 8);
		// printf("Loc: %ld\n", loc);
		// printf("S: %ld\n", s);
		// Marking the start of the sub-memory pool - writing 1 to it
		((char*)wf_mem_block)[ blk_size_index + s - 8 ] = (int)((char*)wf_mem_block)[ blk_size_index + s - 8 ] | mask;

		size_t occup_blks = (size%4)?(size/4 + 1):size/4;

		for (g = 0; g < occup_blks; ++g){
			if (h == 8){
				++s;
				h = 0;
				mask = 1;
			}
			((char*)wf_mem_block)[s] = ((char*)wf_mem_block)[s] | mask;
			++h;
			mask <<= 1;
		}
		// Return location
		return &(wf_mem_block[data_index + loc*4]);
	} else {
		// We could not find the requested free space.
		return NULL;
	}
	return NULL;
}

/*************************************************************** 
 * 					Memory de-allocators					   *
 ***************************************************************/
void best_fit_dealloc(void *ptr) {

	size_t size = get_array_size(bf_mem_block);											// size of the memory block
	size_t data_index = get_first_data_elem(bf_mem_block);								// index of the first data value.
	size_t block_count = get_block_count(bf_mem_block);									// number of mem blocks.
	size_t blk_size_index = (block_count%8)?(block_count/8 + 9):(block_count/8 + 8);	// start of mem-pool-size control data

	char* start = &bf_mem_block[data_index];
	char* _ptr = (char*)ptr;
	size_t ptr_index = _ptr - start; 
	// printf("start: %p\n", start);
	// printf("ptr: %p\n", _ptr);
	// printf("Pointer index: %ld\n", ptr_index);
	// printf("Block size index: %ld\n", blk_size_index);
	// printf("block count: %ld\n", block_count);

	if(ptr_index > size - data_index || (ptr_index % 4) != 0){
		// Invalid pointer
		printf("Invalid pointer - Too big or invalid location.\n");
		return;
	}

	size_t byte_loc = ptr_index/32 + 8;
	int i,
		h = 0,
		mask = 1;
	
	// start from the specific bit
	mask <<= ((ptr_index%32)/4);
	h = ((ptr_index%32)/4);
	while(byte_loc <= block_count + 8){

		for(i = h; i < 8; ++i) {

			if(i == h && byte_loc == (ptr_index/32 + 8)){
				if((((char*)bf_mem_block)[blk_size_index + byte_loc - 8] & mask) != mask) {
					// invalid pointer
					printf("Invalid pointer: %p - No a start of a block.\n", ptr);
					return;
				} else {
					// Cleaning both control and control size bit arrays
					((char*)bf_mem_block)[blk_size_index + byte_loc - 8] = ((char*)bf_mem_block)[blk_size_index + byte_loc - 8] & (~mask);
					((char*)bf_mem_block)[byte_loc] = ((char*)bf_mem_block)[byte_loc] & (~mask);
				}

			} else if(((((char*)bf_mem_block)[blk_size_index + byte_loc - 8] & mask) != mask) && ((((char*)bf_mem_block)[byte_loc] & mask) == mask)) {
				// Keep on deleting 1s ...
				((char*)bf_mem_block)[byte_loc] = ((char*)bf_mem_block)[byte_loc] & (~mask);

			} else {
				// We're done cleaning
				return;
			}
			mask <<= 1;
		}
		mask = 1;
		h = 0;
		byte_loc++;
	}
	return;
}

void worst_fit_dealloc(void *ptr) {

	size_t size = get_array_size(wf_mem_block);											// size of the memory block
	size_t data_index = get_first_data_elem(wf_mem_block);								// index of the first data value.
	size_t block_count = get_block_count(wf_mem_block);									// number of mem blocks.
	size_t blk_size_index = (block_count%8)?(block_count/8 + 9):(block_count/8 + 8);	// start of mem-pool-size control data

	char* start = &wf_mem_block[data_index];
	char* _ptr = (char*)ptr;
	size_t ptr_index = _ptr - start; 

	if(ptr_index > size - data_index || (ptr_index % 4) != 0){
		// Invalid pointer
		printf("Invalid pointer - Too big or invalid location.\n");
		return;
	}

	size_t byte_loc = ptr_index/32 + 8;
	int i,
		h = 0,
		mask = 1;
	
	// start from the specific bit
	mask <<= ((ptr_index%32)/4);
	h = ((ptr_index%32)/4);
	while(byte_loc <= block_count + 8){

		for(i = h; i < 8; ++i) {

			if(i == h && byte_loc == (ptr_index/32 + 8)){
				if((((char*)wf_mem_block)[blk_size_index + byte_loc - 8] & mask) != mask) {
					// invalid pointer
					printf("Invalid pointer: %p - No a start of a block.\n", ptr);
					return;
				} else {
					// Cleaning both control and control size bit arrays
					((char*)wf_mem_block)[blk_size_index + byte_loc - 8] = ((char*)wf_mem_block)[blk_size_index + byte_loc - 8] & (~mask);
					((char*)wf_mem_block)[byte_loc] = ((char*)wf_mem_block)[byte_loc] & (~mask);
				}

			} else if(((((char*)wf_mem_block)[blk_size_index + byte_loc - 8] & mask) != mask) && ((((char*)wf_mem_block)[byte_loc] & mask) == mask)) {
				// Keep on deleting 1s ...
				((char*)wf_mem_block)[byte_loc] = ((char*)wf_mem_block)[byte_loc] & (~mask);

			} else {
				// We're done cleaning
				return;
			}
			mask <<= 1;
		}
		mask = 1;
		h = 0;
		byte_loc++;
	}
	return;
}

/***************************************************************
 * 			Memory algorithm metric utility function(s)		   *
 ***************************************************************/
/* count how many free blocks are less than the input size */
int best_fit_count_extfrag(size_t size) {

	size_t count = 0,
		   ext_cnt = 0,
		   s = 8,
		   blk_cnt = get_block_count(bf_mem_block);
	int i,
		h = 0,
		mask = 1;

	for (i = 0; i < blk_cnt; ++i) {
		if (h == 8) {
			++s;
			h = 0;
			mask = 1;
		}

		if ((((char*)bf_mem_block)[s] & mask) == 0) {
			// found a free block
			count += 4;
		} else {
			// found an occupied block
			if (count > 0 && count < size)
			{
				ext_cnt++;
			}
			count = 0;
		}
		++h;
		mask <<= 1;
	}
	// In case the last bit was free
	if (count > 0 && count < size) {
		ext_cnt++;
	}

	return ext_cnt;
}

int worst_fit_count_extfrag(size_t size) {
	size_t count = 0,
		   ext_cnt = 0,
		   s = 8,
		   blk_cnt = get_block_count(wf_mem_block);
	int i,
		h = 0,
		mask = 1;

	for (i = 0; i < blk_cnt; ++i) {
		if (h == 8) {
			++s;
			h = 0;
			mask = 1;
		}

		if ((((char*)wf_mem_block)[s] & mask) == 0) {
			// found a free block
			count += 4;

		} else {
			// found an occupied block
			if (count > 0 && count < size)
			{
				ext_cnt++;
			}
			count = 0;
		}
		++h;
		mask <<= 1;
	}
	// In case the last bit was free
	if (count > 0 && count < size) {
		ext_cnt++;
	}

	return ext_cnt;
}

/* Returns the size of the memory block. */
size_t get_array_size(char* mem_block) {
	int i;
	size_t size = 0;
	size_t temp = 0;
	unsigned char temp2 = 0;
	for(i = 0; i < 8; ++i){
		temp = (unsigned char)mem_block[i];
		size = size | (temp << (8*i));
	}
	return size;
}

/* Returns the location of the first data bit of the memory block. */
size_t get_first_data_elem(char* mem_block) {

	size_t size = get_array_size(mem_block);
	size_t first = (size - 8) / 34;
	if ((size - 8) % 34 >= 6) {
		++first;
	}
	return (2*first + 8);
}

/* Returns the number of blocks */
size_t get_block_count(char* mem_block) {
	size_t size = get_array_size(mem_block);
	size_t num = (size - 8) / 34;
	num *= 8;
	if((size - 8) % 34 >= 6){
		num += (((size - 8) % 34) - 2) / 4;
	}
	return num;
}

/* For testing purposes. */
void output_values(int in, int ext_frag){
	size_t i;
	int si;
	int mask = 1;
	size_t count = 0;
	//printf("Memory pool status: \n");

	if(in){
		// print worst_fit
		size_t index = get_first_data_elem(wf_mem_block);
		size_t block_cnt = get_block_count(wf_mem_block);
		size_t ctrl_byte_count = (index - 8)/2;

		printf("Block count: %ld\n", block_cnt);
		printf("Memory pool: ");
		for(i = 8; i < ctrl_byte_count + 8; i++){
			for(si = 0; si < 8; si++){
				if(((unsigned char)((char*)wf_mem_block)[i] & mask) == mask){
					count+= 4;
				} else{
					if(count > 0){
						printf(" %ld ", count);
						count = 0;
					}
					printf("%d", 0);
				}
				mask <<= 1;
			}
			mask = 1;
			//printf("%ld-byte: %u\n", i, (unsigned char)((char*)wf_mem_block)[i]);
		}
		if(count > 0){
			printf(" %ld ", count);
			count = 0;
		}
		mask = 1;
		printf("\nStart addresses of mem blocks: ");
		for(i = 8 + ctrl_byte_count; i < index; i++){
			for(si = 0; si < 8; si++){
				if(((unsigned char)((char*)wf_mem_block)[i] & mask) == mask){
					printf("%ld , ", 8*(i - 8 - ctrl_byte_count) + si + 1);
				} 
				mask <<= 1;
			}
			mask = 1;
		}
		size_t ext = worst_fit_count_extfrag(ext_frag);
		printf("\nExternal_frag(%d): %ld", ext_frag, ext);

	} else {
		// print best_fit
		size_t index = get_first_data_elem(bf_mem_block);
		size_t block_cnt = get_block_count(bf_mem_block);
		size_t ctrl_byte_count = (index - 8)/2;

		printf("Block count: %ld\n", block_cnt);
		printf("Memory pool: ");
		for(i = 8; i < ctrl_byte_count + 8; i++){
			for(si = 0; si < 8; si++){
				if(((unsigned char)((char*)bf_mem_block)[i] & mask) == mask){
					count+= 4;
				} else{
					if(count > 0){
						printf(" %ld ", count);
						count = 0;
					}
					printf("%d", 0);
				}
				mask <<= 1;
			}
			mask = 1;
			//printf("%ld-byte: %u\n", i, (unsigned char)((char*)bf_mem_block)[i]);
		}
		if(count > 0){
			printf(" %ld ", count);
			count = 0;
		}
		mask = 1;
		printf("\nStart addresses of mem blocks: ");
		for(i = 8 + ctrl_byte_count; i < index; i++){
			for(si = 0; si < 8; si++){
				if(((unsigned char)((char*)bf_mem_block)[i] & mask) == mask){
					printf("%ld , ", 8*(i - 8 - ctrl_byte_count) + si + 1);
				} 
				mask <<= 1;
			}
			mask = 1;
		}
		size_t ext = best_fit_count_extfrag(ext_frag);
		printf("\nExternal_frag(%d): %ld", ext_frag, ext);
	}

	printf("\n\n");
}