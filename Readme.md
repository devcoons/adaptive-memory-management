###### Version: 0.2-alpha
Adaptive Memory Management Scheme
==================


  - [Synopsis](#synopsis)
  - [Compatibility](#compatibility)
  - [Requirements](#requirements)
  - [Installation](#installation)
  - [API](#api)
  - [Usage](#usage)
  - [Examples](#examples)
  - [Contributing](#contributing)
  - [License](#license)


## Synopsis

This Project is a memory allocation scheme that provides efficient dynamic memory allocation and defragmentation for embedded systems lacking a Memory Management Unit (MMU). Using as main criteria the efficiency in handling both external and internal memory fragmentation, as well as the requirements of soft real-time applications in constraint-embedded systems, the proposed solution of memory management delivers a more precise memory allocation process. 

The proposed Adaptive Memory Management Scheme (AMM) maintains a balance between performance and efficiency, with the objective to increase the amount of usable memory in MMU-less embedded systems with a bounded and acceptable timing behavior. By maximizing memory utilization, embedded systems applications can optimize their performance in time-critical tasks and meet the demands of Internet-of-Things (IoT) solutions, without undergoing memory leaks and unexpected failures. 

Its use requires no hardware MMU, and requires few or no manual changes to application software. The proposed scheme is evaluated providing encouraging results regarding performance and reliability compared to the default memory allocator. Allocation of fixed and random size blocks delivers a speedup ranging from 2x to 5x over the standard GLIBC allocator, while the de-allocation process is only 20% percent slower, but provides a perfect (0%) defragmented memory.

## Compatibility
This is the main Compatibility section, listing all devices that are supported by Adaptive Memory Management Scheme.
<br/><br/>
  - PJRC Teensy Boards (3.0/3.1/3.2) [Using microVisual plugin for Microsoft Visual Studio and Arduino Software 1.6]
  - Arduino Mini (3.3v) [Using microVisual plugin for Microsoft Visual Studio and Arduino Software 1.4/1.5/1.6]
  - Arduino Micro (3.3v) [Using microVisual plugin for Microsoft Visual Studio and Arduino Software 1.4/1.5/1.6]
  - Arduino Uno (5v) [Using microVisual plugin for Microsoft Visual Studio and Arduino Software 1.4/1.5/1.6]
  - AVR 40 Pin 16MHz 32K 8A/D - ATMEGA32A-PU [Arduino Software 1.4/1.5/1.6 using ATMega support]
  - ATMEGA168P-20PU [Arduino Software 1.4/1.5/1.6 using ATMega support]
  - AVR 8 Pin 20MHz 8K 4A/D - ATtiny85 [Arduino Software 1.4/1.5/1.6 using ATtiny support] 
  - Ubuntu 16.04 [Using gcc g++ make]
  - Windows 10 [Using Microsoft Visual Studio 2015 / C++]

## Requirements

[ NONE ]

## Installation

Download the source code and copy both header and source files to your project's folder.
```
$ git clone https://gitlab.com/iikem/adaptive-memory-management  or  download as zip
$ cd "Source Code"/
```
This version can be used either on Arduino based platforms or Windows/Linux.

## API

Initialize a memory space used by the memory manager (only for Windows/Linux). In this case the memory manager will be used as a sub-allocator.
```
void memory_initialize ( size_t );
```
Allocates a memory space and returns the address.
```
void * memory_allocate ( size_t );
```
Reallocates a memory block and returns the address of the new memory space.
```
void * memory_reallocate ( void *, size_t );
```
Registering the address of a pointer to the pointers section of the memory manager. Returns NULL in case of failure.
```
void * memory_pointer_add ( void * );
```
Deregistering the address of a pointer from the pointers section of the memory manager. Returns NULL in case of failure.
```
void * memory_pointer_remove ( void * );
```
Moves a memory block to an other point in memory.
```
void memory_move ( void *, void *, size_t );
```
Returns the size of the memory
```
size_t memory_size ( );
```
Returns the size of the available memory
```
size_t memory_available ( );
```
Returns the size of the data section
```
size_t memory_data_section_size ( );
```
Returns the size of the pointers section
```
size_t memory_pointers_section_size ( );
```
Prints some information about the data section. (data block addresses)
```
void memory_print_data_section ( );
```
Prints some information about the pointers section. (pointer base address/pointer address)
```
void memory_print_pointers_section ( );
```
Prints general memory information
```
void memory_print_information ( );
```

## Usage

Make sure you have included the header file.
```
$ #include "Memory.h"
```
(FOR LINUX/WINDOWS ONLY) initialize the memory manager space (it will be used as a sub-allocator)
```
$ memory_initialize(10000);
```
Step 1: declare a pointer
```
$ char * pointer;
```
Step 2: register the pointer to the memory manager
```
$ memory_pointer_add(&pointer);
```
Step 3: allocate some memory space and use the pointer as normal. (assign values etc)
```
$ pointer = (char *) memory_allocate ( X * sizeof(char) );
$ ...
$ ...
```
Step 4: free the allocated memory space
```
$ memory_free(pointer);
```
Step 4: deregister the pointer from the memory manager
```
$ memory_pointer_remove(&pointer);
```
## Examples
```
[ Under Construction ]
```

## Contributing
One of the easiest ways to contribute is to participate in discussions and discuss issues. You can also contribute by submitting pull requests with code changes. Please log a new issue in the appropriate section [Issues] or by e-mail.
The best way to get your bug fixed is to be as detailed as you can be about the problem.  Providing a minimal project with steps to reproduce the problem is ideal. 

Here are questions you can answer before you file a bug to make sure you're not missing any important information.

1. Did you read the documentation?
2. Did you include the command you executed in the issue?
3. What are the EXACT steps to reproduce this problem?
4. What package versions are you using?

## License

The MIT License (MIT)

Copyright (c) 2016 Io. D

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.