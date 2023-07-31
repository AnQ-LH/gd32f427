/**
 * linker script for GD32F4xx with GNU ld
 * BruceOu 2021-12-14
 */

/* Program Entry, set to mark it as "used" and avoid gc */
MEMORY
{
	RAM (rw) : ORIGIN = 0x20000000, LENGTH =  256k /* 256KB sram */
	FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 1024k /* 3072KB flash */
}

/* Program Entry，The first function that the program starts */
ENTRY(Reset_Handler)
/* define the size of System Stack, it can be modified as needed  */
_system_stack_size = 0x200;

/* define the sections */
SECTIONS
{
	.text :
	{
		. = ALIGN(4);
		_stext = .;					/* Defined as the address of the current location and used to record the start address of the code snippet (text). */
		KEEP(*(.isr_vector))		/* Startup code, Used to define a table of interrupt vectors */
		. = ALIGN(4);
		*(.text)					/* remaining code */
		*(.text.*)					/* remaining code */
		*(.rodata)					/* read-only data (constants) */
		*(.rodata*)					/* read-only data (constants) */
		*(.glue_7)					/* Function call jumps are typically used for the ARM architecture(ARM instructions ) */
		*(.glue_7t)					/* Function call jumps are typically used for the ARM architecture(Thumb instructions ) */
		*(.gnu.linkonce.t*)			/* Allows the linker to merge the same function definitions */

		/* section information for finsh shell */
		. = ALIGN(4);
		__fsymtab_start = .;
		KEEP(*(FSymTab))
		__fsymtab_end = .;
		. = ALIGN(4);
		__vsymtab_start = .;
		KEEP(*(VSymTab))
		__vsymtab_end = .;
		. = ALIGN(4);

		/* section information for initial. */
		. = ALIGN(4);
		__rt_init_start = .;
		KEEP(*(SORT(.rti_fn*)))
		__rt_init_end = .;
		. = ALIGN(4);

		. = ALIGN(4);
		_etext = .;
	} > FLASH = 0

	/* .ARM.exidx is sorted, so has to go in its own output section.  */
	__exidx_start = .;
	.ARM.exidx :
	{
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)

		/* This is used by the startup in order to initialize the .data secion */
		_sidata = .;
	} > FLASH
	__exidx_end = .;

	/* .data section which is used for initialized data */

	.data : AT (_sidata)
	{
		. = ALIGN(4);
		/* This is used by the startup in order to initialize the .data secion */
		_sdata = . ;

		*(.data)
		*(.data.*)
		*(.gnu.linkonce.d*)

		. = ALIGN(4);
		/* This is used by the startup in order to initialize the .data secion */
		_edata = . ;
	} >RAM

	.stack : 
	{
		. = . + _system_stack_size;
		. = ALIGN(4);
		_estack = .;
	} >RAM

	__bss_start = .;
	.bss :
	{
		. = ALIGN(4);
		/* This is used by the startup in order to initialize the .bss secion */
		_sbss = .;

		*(.bss)
		*(.bss.*)
		*(COMMON)

		. = ALIGN(4);
		/* This is used by the startup in order to initialize the .bss secion */
		_ebss = . ;

		*(.bss.init)
	} > RAM
	__bss_end = .;
	
	_end = .;
	end = .;

	/* Stabs debugging sections.  */
	.stab          0 : { *(.stab) }
	.stabstr       0 : { *(.stabstr) }
	.stab.excl     0 : { *(.stab.excl) }
	.stab.exclstr  0 : { *(.stab.exclstr) }
	.stab.index    0 : { *(.stab.index) }
	.stab.indexstr 0 : { *(.stab.indexstr) }
	.comment       0 : { *(.comment) }
	/* DWARF debug sections.
	 * Symbols in the DWARF debugging sections are relative to the beginning
	 * of the section so we begin them at 0.  */
	/* DWARF 1 */
	.debug          0 : { *(.debug) }
	.line           0 : { *(.line) }
	/* GNU DWARF 1 extensions */
	.debug_srcinfo  0 : { *(.debug_srcinfo) }
	.debug_sfnames  0 : { *(.debug_sfnames) }
	/* DWARF 1.1 and DWARF 2 */
	.debug_aranges  0 : { *(.debug_aranges) }
	.debug_pubnames 0 : { *(.debug_pubnames) }
	/* DWARF 2 */
	.debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
	.debug_abbrev   0 : { *(.debug_abbrev) }
	.debug_line     0 : { *(.debug_line) }
	.debug_frame    0 : { *(.debug_frame) }
	.debug_str      0 : { *(.debug_str) }
	.debug_loc      0 : { *(.debug_loc) }
	.debug_macinfo  0 : { *(.debug_macinfo) }
	/* SGI/MIPS DWARF 2 extensions */
	.debug_weaknames 0 : { *(.debug_weaknames) }
	.debug_funcnames 0 : { *(.debug_funcnames) }
	.debug_typenames 0 : { *(.debug_typenames) }
	.debug_varnames  0 : { *(.debug_varnames) }
}
