/*
 * link.ld
 *
 * A concise linker script for bare-metal RISC-V programs.
 * It defines the memory layout and how code/data are loaded into the simulator's memory.
 */

/* Define the program's entry point as the _start function. */
/* In a bare-metal context, _start is the very first C code executed after CPU boot. */
ENTRY(_start)

/* Define the target memory region. */
/* RAM: */
/* - (rwx): Permissions for read, write, and execute. This is often sufficient */
/* for simple bare-metal programs with mixed code and data. */
/* - ORIGIN = 0x0: The starting address of memory. Adjust this based on your */
/* simulator's memory map. */
/* - LENGTH = 256M: The total length of the memory region (256 MB in this example). */
/* Adjust this according to your simulator's memory size. */
MEMORY
{
  RAM (rwx) : ORIGIN = 0x0, LENGTH = 256M
}

/* Define how different program sections (e.g., code, data) are mapped into memory. */
SECTIONS
{
  /* Set the current virtual address (VMA) to the beginning of the RAM region. */
  . = ORIGIN(RAM);

  /* This is a dummy section to force the linker to leave space for the ELF header and PHDRs. */
  /* Its purpose is to control the Load Memory Address (LMA) for the real code section. */
  /* This section itself will be empty or very small, but its LMA will determine
   * where the PHDR segment ends in the file. */
  . = ALIGN(0x1000); /* Align the start of the code/data to a 4KB boundary in memory. */
                     /* This will also align its file offset due to the PHDR setup. */

  /* Define the main .text_segment that will be loaded into memory. */
  /* This segment contains the executable code, read-only data, and initialized data. */
  .text_segment :
  {
    /* Symbol marking the start of the code/data within RAM. */
    PROVIDE(start_of_program = .);

    /* Place the '.text.startup' section first, typically for _start */
    KEEP(*(.text.startup))
    *(.text)               /* All other .text sections (executable code) */
    *(.text.*)             /* Any sub-sections of .text */

    . = ALIGN(4);          /* Align read-only data */
    *(.rodata)             /* Read-only data (e.g., string literals) */
    *(.rodata.*)           /* Any sub-sections of .rodata */

    . = ALIGN(4);          /* Align initialized data */
    *(.data)               /* Initialized read-write data */
    *(.data.*)             /* Any sub-sections of .data */

    . = ALIGN(0x1000);     /* Align the end of this segment to a 4KB boundary */

  } > RAM : text_rodata_data /* CRITICAL: Assign this section to the 'text_rodata_data' PHDR. */

  /* .bss section: Contains uninitialized read-write data. */
  /* (NOLOAD) means it won't be loaded from the ELF file; your simulator or */
  /* startup code needs to zero it out in memory before execution. */
  .bss (NOLOAD) : ALIGN(8)
  {
    *(.bss)
    *(.bss.*)
    *(COMMON)
    . = ALIGN(8);
  } > RAM

  /* Define a general _end symbol, pointing to the end of all allocated sections. */
  /* Your simulator might use this to determine the program's total memory footprint. */
  . = ALIGN(0x1000);
  _end = .;
}

/* Define Program Headers (PHDRS). */
PHDRS
{
  /* This PHDR is specifically for the ELF file header and the Program Header table itself. */
  /* It must start at file offset 0 (AT(0)). */
  /* It is marked PT_LOAD so a loader will actually load these headers into memory. */
  /* The linker will determine its size based on the actual size of headers. */
  headers PT_LOAD FILEHDR PHDRS FLAGS(R) AT (0); /* Read-only permissions for headers */

  /* This PHDR is for the actual code and data sections. */
  /* It will be placed immediately after the 'headers' segment in the file. */
  /* Its VMA (memory address) will be the ORIGIN(RAM), but its LMA (file offset) */
  /* will be automatically determined by the linker to follow the 'headers' segment. */
  /* FLAGS(7): Sets permissions to Read (4) + Write (2) + Execute (1) for code/data. */
  text_rodata_data PT_LOAD FLAGS(7);
}
