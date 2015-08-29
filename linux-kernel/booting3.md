###About GDT
1. GDTR is a 48-bit register and consists of two parts:  
>size(16-bit) of global descriptor table;  
address(32-bit) of the global descriptor table;

2. segment descriptor:
> 64-bit  
**Limit[20-bit] and G flag:** if G is 0, Limit is interpreted in terms of 1 Byte and the
maximum size of the segment can be 1MB. If G is 1, the value of Limit is shifted to
the left by 12 bits. So, Limit is interpreted in terms of 4096 Bytes = 4KB = 1 Page.
Limit = length_of_segment - 1  
**Base[32-bit]:** defines the linear address of the segment's starting location  
**Type/Attribute[8-bit]:** defines the type of segment and kinds of access to it.

3. segment register(16-bit), layout shows below:
        -----------------------------
        |       Index    | TI | RPL |
        -----------------------------
> **Index[13-bit]:** shows the index number of the descriptor in the GDT  
**TI[1-bit]:**  shows where to search for the descriptor. If it is 0 then search in the Global Descriptor Table(GDT) otherwise it will look in Local Descriptor Table(LDT).  
**RPL[2-bit]:** Requester's Privilege Level.  
**Hidden part:** The CPU tries to find a segment descriptor by GDT address + Index from selector and load the descriptor into the hidden part of the segment register

4. Algorithm for the translation from real mode into protected mode:  
    * Disable interrupts
    * Describe and load GDT with lgdt instruction
    * Set PE (Protection Enable) bit in CR0 (Control Register 0)
    * Jump to protected mode code

5. Preparations before switch to protected mode:
    * copy boot params: copy hdr from header.S to the boot_params structure in setup_header filed.
    * console initialization
    * heap initialization
    * CPU validation, memory detection, keyboard initialization, querying. These three steps are
      all get some information of the computer, and store the informations info boot_param structure.
    * set video
