#include "Memory.h"

	#ifndef ARDUINO

		volatile char * memory_starting_address;

		volatile char * memory_ending_address;

		volatile char * memory_data_section_starting;

		volatile char * memory_data_section_ending;

		volatile char * memory_pointers_section_starting;

		volatile char * memory_pointers_section_ending;

		volatile memory_data_block_t * memory_last_data_block;

	#else

		volatile char * memory_starting_address = ( volatile char * ) &_ebss + 15000;

		volatile char * memory_ending_address = ( volatile char * ) &_estack - DATA_BLOCK_MARGIN;

		volatile char * memory_pointers_section_starting = ( volatile char * ) &_ebss + 10000;

		volatile char * memory_pointers_section_ending = ( volatile char * ) &_ebss + 10000;

		volatile char * memory_data_section_starting = ( volatile char * ) &_ebss + 10000+POINTER_BLOCK_MARGIN;

		volatile char * memory_data_section_ending = ( volatile char * ) &_ebss + 10000 + POINTER_BLOCK_MARGIN;

		volatile memory_data_block_t * memory_last_data_block = ( volatile memory_data_block_t * ) &memory_last_data_block;

	#endif


	#ifndef ARDUINO

		void memory_initialize ( size_t size )
		{
			memory_starting_address = ( volatile char * ) malloc ( size * sizeof ( char ) );

			memory_ending_address = ( volatile char * ) memory_starting_address + size - 1;

			memory_pointers_section_starting = memory_starting_address;

			memory_pointers_section_ending = memory_starting_address;

			memory_data_section_starting = ( volatile char * ) memory_pointers_section_ending + 0;

			memory_data_section_ending = ( volatile char * ) memory_pointers_section_ending + 0;

			memory_last_data_block = ( volatile memory_data_block_t * ) &memory_last_data_block;
		}

	#endif


	void * memory_allocate ( size_t size )
	{
		size = ALIGN_SIZE ( size );

		intptr_t inbetween_sections_space = ( intptr_t ) memory_data_section_starting - ( size + sizeof ( memory_data_block_t ) + ( intptr_t ) memory_pointers_section_ending );

		intptr_t end_of_data_section_space = ( intptr_t ) memory_ending_address - ( size + sizeof ( memory_data_block_t ) + ( intptr_t ) memory_data_section_ending + DATA_BLOCK_MARGIN );

		if ( ( POINTER_BLOCK_MARGIN < inbetween_sections_space ) || ( ( end_of_data_section_space < 0 ) && ( inbetween_sections_space > 0 ) ) )
		{
			if ( ( memory_data_section_starting == memory_data_section_ending ) && ( ( intptr_t ) memory_data_section_starting - ( intptr_t ) memory_pointers_section_ending ) > POINTER_BLOCK_MARGIN )
			{
				memory_data_section_starting = ( volatile char * ) memory_pointers_section_ending + POINTER_BLOCK_MARGIN;

				memory_data_section_ending = ( volatile char * ) memory_pointers_section_ending + POINTER_BLOCK_MARGIN;

				return memory_allocate ( size );
			}

			memory_data_block_t * new_block = ( memory_data_block_t * ) ( ( volatile char * ) memory_data_section_starting - ( size + sizeof ( memory_data_block_t ) ) );

			new_block->size = size;

			new_block->previous = ( memory_data_block_t * ) &memory_last_data_block;

			( ( memory_data_block_t * ) memory_data_section_starting )->previous = new_block;

			memory_data_section_starting = ( volatile char * ) memory_data_section_starting - ( size + sizeof ( memory_data_block_t ) );

			return ( ( char * ) new_block + sizeof ( memory_data_block_t ) );
		}

		if ( end_of_data_section_space > 0 )
		{
			memory_data_block_t * new_block = ( memory_data_block_t * ) memory_data_section_ending;

			memory_data_section_ending = ( volatile char * ) memory_data_section_ending + sizeof ( memory_data_block_t ) + size;

			new_block->size = size;

			new_block->previous = ( memory_data_block_t * ) memory_last_data_block;

			memory_last_data_block = new_block;

			return ( ( char * ) new_block + sizeof ( memory_data_block_t ) );
		}

		return NULL;
	}


	void * memory_reallocate ( void * pointer, size_t size )
	{
		if ( *( ( char** ) pointer ) == NULL )
		{
			*( char** ) pointer = ( char * ) memory_allocate ( size );

			return NULL;
		}

		size = ALIGN_SIZE ( size );

		char * new_pointer;

		new_pointer = ( char * ) memory_allocate ( size );

		intptr_t saddress = ( intptr_t )*( char** ) pointer;

		intptr_t eaddress = ( ( intptr_t )*( char** ) pointer ) + ( intptr_t ) ( ( memory_data_block_t * ) ( ( *( ( char** ) pointer ) ) - sizeof ( memory_data_block_t ) ) )->size - 2;

		int offset = ( ( intptr_t ) new_pointer - ( intptr_t )*( char** ) pointer );

		char * old_place = *( char** ) pointer;

		char * old_new_pointer = new_pointer;

		*( char** ) pointer = new_pointer;

		_memory_pointers_update_value ( ( char * ) saddress, ( char * ) eaddress, offset );

		memmove ( old_new_pointer, old_place, size <= ( ( memory_data_block_t * ) ( ( old_place ) -sizeof ( memory_data_block_t ) ) )->size ? size : ( ( memory_data_block_t * ) ( ( old_place ) -sizeof ( memory_data_block_t ) ) )->size );

		_memory_pointers_update_address ( ( char * ) saddress, ( char * ) eaddress, offset );

		memory_free ( old_place );

		return ( char* ) new_pointer + offset;
	}


	void * memory_free ( void * pointer )
	{
		if ( pointer == NULL )

			return NULL;

		volatile memory_data_block_t * data_node = ( volatile memory_data_block_t * ) ( ( intptr_t ) pointer - sizeof ( memory_data_block_t ) );

		volatile char ** current_pointer = ( volatile char ** ) memory_pointers_section_starting;

		do
		{
			if ( ( ( ( uintptr_t ) *current_pointer ) >= ( ( uintptr_t ) data_node ) ) && ( ( uintptr_t ) *current_pointer ) <= ( ( ( uintptr_t ) data_node ) + sizeof ( memory_data_block_t ) + ( ( uintptr_t ) data_node->size ) ) )
			{
				memory_pointers_section_ending = ( volatile char * ) ( ( volatile char * ) memory_pointers_section_ending - sizeof ( volatile char * ) );

				if ( current_pointer != ( volatile char ** ) memory_pointers_section_ending )
				{
					_memory_pointer_block_move ( ( void * ) current_pointer, ( void * ) ( volatile char * ) memory_pointers_section_ending );

					current_pointer--;
				}
			}
		}

		while ( ( current_pointer = current_pointer + 1 ) <= ( volatile char ** ) memory_pointers_section_ending );
		
		if ( ( intptr_t ) data_node == ( intptr_t ) memory_data_section_starting )
		{
			if ( memory_last_data_block != data_node )
			{
				( ( memory_data_block_t * ) ( ( volatile char* ) data_node + sizeof ( memory_data_block_t ) + data_node->size ) )->previous = data_node->previous;
			}
			else
			{
				memory_last_data_block = ( memory_data_block_t * ) &memory_last_data_block;
			}

			memory_data_section_starting = ( ( volatile char* ) data_node + sizeof ( memory_data_block_t ) + data_node->size );

			return NULL;
		}

		if ( ( ( uintptr_t ) data_node + data_node->size + sizeof ( memory_data_block_t ) ) == ( uintptr_t ) memory_data_section_ending )
		{
			memory_last_data_block = data_node->previous;

			memory_data_section_ending = ( volatile char* ) data_node;

			return NULL;
		}

		size_t data_node_size = data_node->size + sizeof ( memory_data_block_t );

		memory_last_data_block = ( memory_data_block_t * ) ( ( ( intptr_t ) memory_last_data_block ) - data_node_size );

		( ( memory_data_block_t * ) ( ( volatile char* ) data_node + sizeof ( memory_data_block_t ) + data_node->size ) )->previous = data_node->previous;

		_memory_data_blocks_bulk_move ( ( void* ) data_node, ( void* ) ( ( intptr_t ) data_node + data_node_size ), ( ( intptr_t ) memory_data_section_ending - ( ( ( intptr_t ) data_node + data_node_size ) ) ) );

		memory_data_section_ending = ( volatile char * ) ( ( ( intptr_t ) memory_data_section_ending ) - data_node_size );

		memory_data_block_update_previous ( ( memory_data_block_t * ) data_node, ( memory_data_block_t * ) memory_last_data_block, data_node_size*( -1 ) );

		return NULL;
	}


	void memory_data_block_update_previous ( memory_data_block_t * from_data_block, memory_data_block_t * until_data_block, int offset )
	{
		if ( memory_data_section_starting == memory_data_section_ending || from_data_block >= until_data_block )
		{
			return;
		}

		memory_data_block_t * old_previous = from_data_block;

		while ( ( from_data_block = ( memory_data_block_t * ) ( ( ( intptr_t ) from_data_block ) + from_data_block->size + sizeof ( memory_data_block_t ) ) ) <= until_data_block )
		{

			from_data_block->previous = old_previous;

			old_previous = from_data_block;
		}
	}


	void * memory_pointer_add ( void * pointer_address )
	{
		if ( pointer_address == NULL )

			return NULL;

		if ( ( uintptr_t ) memory_pointers_section_ending + sizeof ( void ** ) >= ( uintptr_t ) memory_data_section_starting )
		{

			if ( ( intptr_t ) memory_data_section_starting == ( intptr_t ) memory_data_section_ending )
			{
				memory_data_section_starting = ( volatile char * ) ( ( intptr_t ) memory_data_section_starting + 40*sizeof ( memory_pointer_block_t ) );

				memory_data_section_ending = ( volatile char * ) ( ( intptr_t ) memory_data_section_ending + 40* sizeof ( memory_pointer_block_t ) );

				return memory_pointer_add ( pointer_address );
			}

			memory_data_block_t * first_data_block = ( memory_data_block_t * ) memory_data_section_starting;

			intptr_t end_of_data_section_space = ( intptr_t ) memory_ending_address - ( first_data_block->size + sizeof ( memory_data_block_t ) + ( intptr_t ) memory_data_section_ending + DATA_BLOCK_MARGIN );

			if ( end_of_data_section_space <= 0 )

				return NULL;

			intptr_t first_node_offset = ( ( intptr_t ) pointer_address <= ( intptr_t ) first_data_block + first_data_block->size + sizeof ( memory_data_block_t ) && ( intptr_t ) pointer_address >= ( intptr_t ) first_data_block ) ? ( intptr_t ) memory_data_section_ending - ( intptr_t ) memory_data_section_starting : 0;

			( ( memory_data_block_t* ) ( ( intptr_t ) first_data_block + first_data_block->size + sizeof ( memory_data_block_t ) ) )->previous = ( memory_data_block_t * ) &memory_last_data_block;

			first_data_block->previous = ( memory_data_block_t * ) memory_last_data_block;

			_memory_data_block_move ( ( char * ) memory_data_section_ending, ( char * ) first_data_block );

			memory_last_data_block = ( memory_data_block_t* ) memory_data_section_ending;

			memory_data_section_starting = ( volatile char * ) ( ( intptr_t ) memory_data_section_starting + sizeof ( memory_data_block_t ) + first_data_block->size );

			memory_data_section_ending = ( volatile char * ) ( ( intptr_t ) memory_data_section_ending + sizeof ( memory_data_block_t ) + first_data_block->size );

			return memory_pointer_add ( ( char* ) ( ( ( intptr_t ) pointer_address ) + first_node_offset ) );
		}

		volatile char ** current_dpointer = ( volatile char ** ) memory_pointers_section_ending;

		*current_dpointer = ( volatile char* ) pointer_address;

		//*((char**)pointer_address) = NULL;

		memory_pointers_section_ending = ( volatile char * ) ( ( intptr_t ) memory_pointers_section_ending + sizeof ( memory_pointer_block_t ) );

		return ( void * ) ( ( intptr_t ) memory_pointers_section_ending - sizeof ( memory_pointer_block_t ) );
	}


	void * memory_pointer_remove ( void * pointer_address )
	{
		if ( pointer_address == NULL || memory_pointers_section_starting == memory_pointers_section_ending )

			return NULL;

		volatile char ** current_pointer = ( volatile char ** ) memory_pointers_section_starting;

		do
		{
			if ( *current_pointer == pointer_address )
			{
				memory_pointers_section_ending = ( volatile char * ) ( ( intptr_t ) memory_pointers_section_ending - sizeof ( void ** ) );

				if ( current_pointer != ( volatile char ** ) memory_pointers_section_ending )

					_memory_pointer_block_move ( ( char* ) current_pointer, ( char* ) memory_pointers_section_ending );

				return pointer_address;
			}

			current_pointer += 1;

		}
		while ( ( ( volatile char ** ) current_pointer ) < ( volatile char ** ) memory_pointers_section_ending );

		return NULL;
	}


	void memory_move ( void * destination, void * source, size_t size )
	{

		_memory_data_blocks_bulk_move ( destination, source, size );

	}


	void _memory_data_blocks_bulk_move ( void *destination, void * source, size_t size )
	{
		intptr_t saddress = ( intptr_t ) source;

		intptr_t eaddress = ( intptr_t ) source + size - 1;

		int offset = ( ( intptr_t ) destination - ( intptr_t ) source );

		_memory_pointers_update_value ( ( void * ) saddress, ( void * ) eaddress, offset );

		if ( destination != memmove ( destination, source, size ) );

		_memory_pointers_update_address ( ( void * ) saddress, ( void * ) eaddress, offset );
	}


	void _memory_data_block_move ( void * destination, void * source )
	{
		intptr_t saddress = ( intptr_t ) source;

		intptr_t eaddress = ( intptr_t ) source + ( ( memory_data_block_t * ) source )->size + sizeof ( memory_data_block_t ) - 1;

		int offset = ( ( intptr_t ) destination - ( intptr_t ) source );

		_memory_pointers_update_value ( ( void * ) saddress, ( void * ) eaddress, offset );

		if ( destination != memmove ( destination, source, ( ( memory_data_block_t * ) source )->size + sizeof ( memory_data_block_t ) ) );

		_memory_pointers_update_address ( ( void * ) saddress, ( void * ) eaddress, offset );
	}


	void _memory_pointer_block_move ( void * destination, void * source )
	{
		if ( destination != memmove ( destination, source, sizeof ( void ** ) ) );

		memset ( source, '\0', sizeof ( void ** ) );
	}


	void _memory_pointers_update_address ( void * s_address, void * e_address, intptr_t offset )
	{
		volatile char ** current_pointer = ( volatile char** ) memory_pointers_section_starting;

		do
		{
			if ( ( *current_pointer ) >= ( volatile char * ) s_address && *current_pointer <= ( volatile char * ) e_address )

				*current_pointer = ( volatile char* ) ( ( ( intptr_t ) *current_pointer ) + offset );
		}

		while ( ( current_pointer = current_pointer + 1 ) != ( volatile char ** ) memory_pointers_section_ending );
	}


	void _memory_pointers_update_value ( void * s_address, void * e_address, intptr_t offset )
	{
		volatile char ** current_pointer = ( volatile char ** ) memory_pointers_section_starting;

		do
		{
			if ( ( intptr_t )*( volatile char ** ) ( *( volatile char ** ) current_pointer ) >= ( intptr_t ) ( volatile char * ) s_address && ( intptr_t )*( volatile char ** ) ( *( volatile char ** ) current_pointer ) <= ( intptr_t ) ( volatile char * ) e_address )

				*( volatile char ** ) ( *( volatile char ** ) current_pointer ) = *( volatile char ** ) ( *( volatile char ** ) current_pointer ) + offset;

		}

		while ( ( current_pointer = current_pointer + 1 ) != ( volatile char ** ) memory_pointers_section_ending );
	}


	size_t memory_size ( )
	{
		return ( size_t ) ( ( intptr_t ) memory_ending_address - ( intptr_t ) memory_starting_address );
	}


	size_t memory_available ( )
	{
		return ( memory_size ( ) - ( memory_data_section_size ( ) + memory_pointers_section_size ( ) ) );
	}


	size_t memory_data_section_size ( )
	{
		return ( size_t ) ( ( intptr_t ) memory_data_section_ending - ( intptr_t ) memory_data_section_starting );
	}


	size_t memory_pointers_section_size ( )
	{
		return ( size_t ) ( ( intptr_t ) memory_pointers_section_ending - ( intptr_t ) memory_pointers_section_starting );
	}


	#ifdef ARDUINO

		void memory_print_information ( )
		{
			Serial.println ( );
			
			Serial.print ( "[ Memory ] Size:" );
			
			Serial.print ( memory_size ( ) );
			
			Serial.print ( "  Occupied : " );
			
			Serial.print ( memory_data_section_size ( ) + memory_pointers_section_size ( ) );
			
			Serial.print ( "  Free : " );
			
			Serial.print ( memory_available ( ) );
			
			Serial.print ( "  Data Size : " );
			
			Serial.print ( memory_data_section_size ( ) );
			
			Serial.print ( "  Pointers Size : " );
			
			Serial.println ( memory_pointers_section_size ( ) );
			
			Serial.println ( );
		}

		
		void memory_print_pointers_section ( )
		{
			Serial.println ( );
			
			Serial.print ( "[ POINTERS LIST ] Starting : " );
			
			Serial.print ( ( intptr_t ) memory_pointers_section_starting );
			
			Serial.print ( "  Ending : " );
			
			Serial.println ( ( intptr_t ) memory_pointers_section_ending );
			
			Serial.println ( );

			if ( memory_pointers_section_ending == memory_pointers_section_starting )
			{
				Serial.println ( " - NONE" );
				
				return;
			}

			volatile char ** current_pointer = ( volatile char** ) memory_pointers_section_starting;

			do
			{
				Serial.print ( " - " );
				
				Serial.print ( ( intptr_t ) current_pointer );
				
				Serial.print ( " -> " );
				
				Serial.print ( ( intptr_t ) *current_pointer );
				
				Serial.print ( " -> " );
				
				Serial.println ( ( intptr_t )*( ( volatile char** ) ( *current_pointer ) ) );
			}
			while ( ( current_pointer = current_pointer + 1 ) != ( volatile char** ) memory_pointers_section_ending );
			
			Serial.println ( "[ END ]" );

		}


		void memory_print_data_section ( )
		{
			Serial.println ( );
			
			Serial.print ( "[ DATA LIST ] Starting : " );
			
			Serial.print ( ( intptr_t ) memory_data_section_starting );
			
			Serial.print ( "  Ending : " );
			
			Serial.println ( ( intptr_t ) memory_data_section_ending );
			
			Serial.println ( );

			if ( memory_data_section_ending == memory_data_section_starting )
			{
				Serial.println ( " - NONE" );
				
				return;
			}

			memory_data_block_t* current_data = ( memory_data_block_t* ) memory_data_section_starting;

			do
			{
				Serial.print ( " - At :" );
				
				Serial.print ( ( intptr_t ) current_data );
				
				Serial.print ( " Previous : " );
				
				Serial.print ( ( intptr_t ) current_data->previous );
				
				Serial.print ( " Data Size : " );
				
				Serial.print ( ( intptr_t ) ( intptr_t ) current_data->size );
				
				Serial.print ( " Data Starting : " );
				
				Serial.print ( ( intptr_t ) current_data + sizeof ( memory_data_block_t ) );
				
				Serial.print ( " Ending at : " );
				
				Serial.println ( ( intptr_t ) current_data + sizeof ( memory_data_block_t ) + current_data->size - 1 );
			}
			while ( ( current_data = ( memory_data_block_t* ) ( ( volatile char* ) current_data + sizeof ( memory_data_block_t ) + current_data->size ) ) != ( memory_data_block_t* ) memory_data_section_ending );
			
			Serial.println ( "[ END ]" );
		}

	#endif

	#ifndef ARDUINO
	
		void memory_print_information ( )
		{
			printf ( "[ Memory ] Size:%lu  Occupied:%lu  Free:%lu  Data Size:%lu  Pointers Size:%lu\n", memory_size ( ), memory_data_section_size ( ) + memory_pointers_section_size ( ), memory_available ( ), memory_data_section_size ( ), memory_pointers_section_size ( ) );
		}
		
		
		void memory_print_pointers_section ( )
		{
			printf ( "[ POINTERS LIST ] Starting : %lu (%x)  Ending : %lu (%x)\n", ( intptr_t ) memory_pointers_section_starting, ( intptr_t ) memory_pointers_section_starting, ( intptr_t ) memory_pointers_section_ending, ( intptr_t ) memory_pointers_section_ending );
			
			volatile char ** current_pointer = ( volatile char** ) memory_pointers_section_starting;

			if ( memory_pointers_section_ending == memory_pointers_section_starting )
			{
				printf ( " - NONE\n" );
				
				return;
			}

			do
			{
				printf ( " - %x -> %x -> %x\n", ( intptr_t ) current_pointer, ( intptr_t ) *current_pointer, ( intptr_t )*( ( volatile char** ) ( *current_pointer ) ) );
			}
			while ( ( current_pointer = current_pointer + 1 ) != ( volatile char** ) memory_pointers_section_ending );
			
			printf ( "[ END ]\n" );
		}


		void memory_print_data_section ( )
		{
			printf ( "[ DATA LIST ] Starting : %lu (%x) Ending :%lu (%x) Last Block : %lu (%x)\n", ( intptr_t ) memory_data_section_starting, ( intptr_t ) memory_data_section_starting, ( intptr_t ) memory_data_section_ending, ( intptr_t ) memory_data_section_ending, ( intptr_t ) memory_last_data_block, ( intptr_t ) memory_last_data_block );
			
			memory_data_block_t* current_data = ( memory_data_block_t* ) memory_data_section_starting;
			
			if ( memory_data_section_ending == memory_data_section_starting )
			{
				printf ( " - NONE\n" );
				
				return;
			}

			do
			{
				printf ( " - At :%x  Previous: %x  Data Size : %x  Data Starting : %x  Ending at %x\n", ( intptr_t ) current_data, ( intptr_t ) current_data->previous, ( intptr_t ) current_data->size, ( intptr_t ) current_data + sizeof ( memory_data_block_t ), ( intptr_t ) current_data + sizeof ( memory_data_block_t ) + current_data->size - 1 );
			}
			while ( ( current_data = ( memory_data_block_t* ) ( ( volatile char* ) current_data + sizeof ( memory_data_block_t ) + current_data->size ) ) != ( memory_data_block_t* ) memory_data_section_ending );
			
			printf ( "[ END ]\n" );
		}

	#endif

