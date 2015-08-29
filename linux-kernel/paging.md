####Higher Half Kernel
It is traditional and generally good to have your kernel mapped in every user process. Linux, for instance (and many other Unices) reside at the virtual addresses 0xC0000000 - 0xFFFFFFFF of every address space, leaving the range 0x00000000 - 0xBFFFFFFF for user code, data, stacks, libraries, etc  
see: *http://wiki.osdev.org/Higher_Half_Kernel*

####Page Directory and Page table
1. The entry of Page Directory and Page table are much alike. They both have the 20 most significant bits to be the physics address of a `page`. The number of bits is 20 because of each Page Table and Page Frame is 4K aligned. So the low 12 bits of the physics address must be 0.  
2. note that we use middle 10 bits of the linear address to index a Page Table, however we use lowest 12 bits to index a location inside a page frame. The reason is that for a page frame the access granularity is 1 byte while for a page table of which is 4 bytes(each entry in Page Table is 4 bytes). That why we can use 10 bits rather than 12 bits to index 4kb size memory.
3. Lowest 12 bits of Page Directory and Page Table are some flags which will used by OS, such as present flag, access flag, etc
4. The reason to use multi-level paging is to reduce the memory needed to store the page table. Page Table will not established until needed. 
