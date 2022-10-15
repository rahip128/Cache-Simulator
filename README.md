# Cache-Simulator



first.c will only simulate a one level cache; i.e., an L1 cache.

  •The cache size, associativity, the replacement policy, and the block size are input 
  parameters. Cache size and block size are specified in bytes.
  •You have to simulate a write through cache.
  •Replacement algorithm: You have to support two replacement policies. The two re-placement policies are First In First Out (FIFO)
  and Least Recently Used (LRU).
  

In the second part of the program (second.c), I have simulated a system with a two-level of cache (i.e. L1 and L2). Multi-level caches
can be designed in various ways depending on whether the content of one cache is present in
other levels or not. 

  •In this assignment I implemented an exclusive cache: the lower level cache (i.e. L2) 
  contains only blocks that are not present in the upper level cache (i.e. L1).
  
The input to the cache simulator is a memory access trace, which we have generated by
executing real programs. The trace contains memory addresses accessed during program
execution. Your cache simulator will have to use these addresses to determine if the access
is a hit or a miss, and the actions to perform in each case. The memory trace file consists
of multiple lines. Each line of the trace file corresponds to a memory access performed by
the program. Each line consists of two columns, which are space separated. First column
lists whether the memory access is a read (R) or a write (W) operation. The second column
reports the actual 48-bit memory address that has been accessed by the program.
Here is a sample trace file.
R 0x9cb3d40
W 0x9cb3d40
R 0x9cb3d44
W 0x9cb3d44
R 0xbf8ef498
