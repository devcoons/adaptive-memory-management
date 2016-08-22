#pragma once

#ifndef AMM_H

	#define AMM_H
	
	#define AMM_VERSION_MAJOR 0
	
	#define AMM_VERSION_MINOR 2
	
	#define AMM_VERSION_RELEASE 27
	
	#include <stdio.h>
	
	#include <stdlib.h>
	
	#include <inttypes.h>
	
	#include <string.h>

	#ifdef ARDUINO
		#include "Arduino.h"
	#endif

	#ifdef __cplusplus
		extern "C" {
	#endif

		#define ALIGN_SIZE( sizeToAlign ) (((sizeToAlign) + 3) & ~(3))

		#define BLOCK_STATE_FREE( a )  ((a)|=(1<<0))

		#define BLOCK_STATE_OCCP( a )  ((a)&=~(1<<0))

		#define BLOCK_STATE_REVERSE( a ) ((a) ^= (1<<0))

		#define BLOCK_STATE_CHECK_FREE( a ) ((a) & (1<<0))

		#define BLOCK_STATE_CHECK_OCCP( a ) ((a) & (1<<1))

		#define POINTER_BLOCK_MARGIN 32

		#define DATA_BLOCK_MARGIN 128

		#define MEMORY_DEBUG 0

		typedef struct memory_pointer_block_t
		{
			void * pointer;
		}
		memory_pointer_block_t;

		typedef struct memory_data_block_t
		{
			size_t size;

			struct memory_data_block_t * previous;
		}
		memory_data_block_t;

		extern long _ebss;

		extern long _estack;

		extern volatile char * memory_starting_address;

		extern volatile char * memory_ending_address;

		extern volatile char * memory_data_section_starting;

		extern volatile char * memory_data_section_ending;

		extern volatile char * memory_pointers_section_starting;

		extern volatile char * memory_pointers_section_ending;

		extern volatile memory_data_block_t * memory_last_data_block;


		#ifndef ARDUINO

			void memory_initialize ( size_t );

		#endif

		void * memory_free ( void * );

		void * memory_allocate ( size_t );

		void * memory_reallocate ( void *, size_t );

		void * memory_pointer_add ( void * );

		void * memory_pointer_remove ( void * );

		void _memory_data_block_move ( void *, void * );

		void _memory_pointer_block_move ( void *, void * );

		void _memory_data_blocks_bulk_move ( void *, void *, size_t );

		void _memory_pointers_update_address ( void *, void *, intptr_t );

		void _memory_pointers_update_value ( void *, void *, intptr_t );

		void memory_data_block_update_previous ( memory_data_block_t *, memory_data_block_t *, int );

		void memory_update_margins ( );

		void memory_move ( void *, void *, size_t );

		size_t memory_size ( );

		size_t memory_available ( );

		size_t memory_data_section_size ( );

		size_t memory_pointers_section_size ( );

		void memory_print_data_section ( );

		void memory_print_pointers_section ( );

		void memory_print_information ( );


	#ifdef __cplusplus
	}
	#endif

#endif

