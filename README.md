# Processes-SharedMemory
Shared memory by processes in Linux (POSIX) and Windows (native windows shared memory)

The womework for laboratory no.8 at Operating Systems was to implementing an obiect mapped in the memory to facilitate communication between two processes which count from 1 to 1000 in memory, as following:
- read the memory;
- throw a coin (face 1 and 2). While face = 2,  write in memory next number (incrementing).

The shared memory used by many processes ideea is :
-	create a shared memory zone;
-	this (or part of this) is mapped as memory region within other processes;
-	using a semaphore, allow each process to write in shared memory.

For both Linux (POSIX) and Windows (windows.h) were used BOOST libraries (www.boost.org) to create/open/mapped shared memory.
The semaphore was also implemented in a shared memory area.
