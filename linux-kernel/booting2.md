**What is TSS(Task State Segment)?**  
**What is GDT, LDT and IDT ?**
###Common boot sequence of x86s CPU:
1. immediately after power on, CPU's CS:IP set to a hardware defined Reset Vector. for 32-bit or 64-bit CPU, this vector is: 0xFFFF000:0xFFF0. So the linear address will be 0xFFFFFFF0, usually pointing to the BIOS entry point inside ROM. And this memory location typically contains a jump instruction that transfers execution to the location of the BIOS start-up program.

2. the BIOS program runs a power-on self-test to check and initialize required devices. Then it will find a bootable devices and load MBR to linear address 0x7c00 and transfer execution to the boot code.

3. In MBR, there typically leave 434 to 446 bytes available for machine code of a boot loader. So Boot loaders with bigger footprints(such as Grub) are thus split into pieces, where the smallest piece fits into and resides within the MBR, while larger piece(s) are stored in other locations and invoked by the boot loader's MBR code. For example, GRUB 2's boot.img resides in MBR which containing a pointer pointer that is uses to jump to the location of GRUB 2's core image. The core image begins with diskboot.img, which is usually stored immediately after the first sector in the unused space before the first partition. The above code loads the rest of the core image into memory, which contains GRUB 2's kernel and drivers for handling file systems. refer to:  
*https://en.wikipedia.org/wiki/Booting*  
*https://en.wikipedia.org/wiki/GNU_GRUB*

4. then the boot loader load kernel images into memory, uncompress it and transfer the execution to kernel code.
