###Some conclusions:  
    16bits <=> 65536 <=> 64KB
    20bits <=> 1MB
    24bits <=> 16MB
    32bits <=> 4GB  

###Real mode:
>In real mode, a segment is always 65536 bytes in size. Because the address register is 16-bit.
CS, DS, SS, ES are all segment register. When fetch an address in physics memory, the segment address
is always shifted by 4-bit and then added to a 16-bit offset to yield a linear address.

1. A single linear address can be mapped to 4096 [segment : offset] pairs.  
**Proof:**  
    In real mode, each address which is a multiple of 16 bytes called paragraph. And each paragraph can
be segment. Given a segment, we can using the offset to address 64KB memory space. And in 64KB size,
there exist 64KB / 16 = 4KB = 4096 valid paragraph, All this 4096 different segment with correspond offset
can access the same memory address.
2. A20 line wrap. In 80x CPU, there are total 20 address lines. The max segment is 0xFFFF, so the valid
offset when segment is 0xFFFF will be 0x0 - 0xF. When offset is larger than 0xF, the memory address it represented
will beyond 1MB. But in some machine, when offset is larger than 0xF, it ignorance the 20-bit so to make the
address wrapped in to the begin. For example, when offset is 0x10, segment is 0xFFFF, so address = segment : offset
= 0xFFFF0 + 0x10 = 0x100000. When disable 20-bit, address become 0x00000. This called wrap around.
3. In Real mode, it said that segment size is 64KB is not mean that each segment is exact 64KB. Actually, since
each paragraph can be segment, segments can be overlap. And CPU doesn't provide any method to check validity of the
limit of a segment. It is all program's responsibility to maintain the segment infos.

###some abbreviation:
    RPL -> Requested Privilege Level
    CPL -> Current Privilege Level
    DPL -> Descriptor Privilege Level
    GDT -> Global Descriptor Table
    LDT -> Local Descriptor Table
    IDT -> Interrupt Descriptor Table

###Protected mode:
1. In Real mode
when you write any 16-bit value to a segment register, that value multiply by 16 added to offset get linear
    address.
you can't visit the limit and attribute part of segment register. But it seemed that when write to segment
    register(selector part) the hidden base part will auto synced.

2. In Protected mode with paging enabled, address translation goes through:
        Logical Address --> Linear Address --> Physical Address
write to a segment register causes a descriptor to be fetched from the GDT or LDT and unpacked into the base,
    limit and attribute portion of the segment register.

3. Flat mode.
For modern segment register is large enough, disable the segmentation phase. so the logical address is the linear
    address

4. register cr0's 0-bit controlls the switch between Real mode and Protected mode.

5. for more information, refer to the following links:
*http://geezer.osdevbrasil.net/johnfine/segments.htm*
*http://duartes.org/gustavo/blog/post/memory-translation-and-segmentation/*
*http://www.sudleyplace.com/pmtorm.html*
*https://en.wikipedia.org/wiki/X86_memory_segmentation*

6. When switching between real mode and protected mode, since the paging configuration is the same and also, the physical
address is the same too. So is this mean that the linear address between the real and protected mode is the same? And the
advantages of protected mode are providing large memory space and some memory visiting protection?
