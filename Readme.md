#### Current Version
```
0.2:alpha
```
## Synopsis
```
This Project is a memory allocation scheme that provides efficient dynamic memory allocation and defragmentation for embedded systems lacking a Memory Management Unit (MMU). 
Using as main criteria the efficiency in handling both external and internal memory fragmentation, as well as the requirements of soft real-time 
applications in constraint-embedded systems, the proposed solution of memory management delivers a more precise memory allocation process. 

The proposed Adaptive Memory Management Scheme (AMM) maintains a balance between performance and efficiency, 
with the objective to increase the amount of usable memory in MMU-less embedded systems with a bounded and acceptable timing behavior. 
By maximizing memory utilization, embedded systems applications can optimize their performance in time-critical tasks and meet 
the demands of Internet-of-Things (IoT) solutions, without undergoing memory leaks and unexpected failures. 

Its use requires no hardware MMU, and requires few or no manual changes to application software. 
The proposed scheme is evaluated providing encouraging results regarding performance and reliability compared to the default memory allocator. 
Allocation of fixed and random size blocks delivers a speedup ranging from 2x to 5x over the standard GLIBC allocator, 
while the de-allocation process is only 20% percent slower, but provides a perfect (0%) defragmented memory.

```
## Requirements
```
[ Empty ]
```
## Installation
```
$ git clone https://gitlab.com/iikem/adaptive-memory-management
$ cd "Source Code"/
```
## Usage
```
$ char * pointer;
$ memory_pointer_add(&pointer);
$ pointer = (char *) memory_allocate ( X * sizeof(char) );
$ ...
$ ...
$ memory_free(pointer);
$ memory_pointer_remove(&pointer);
```
## Example
```
[ Empty ]
```
## License
```
Please check the LICENSE.md file
```