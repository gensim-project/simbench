SECTIONS 
{
	. = 0x0;
	.vectors : 
	{
		*vectors.o;
	}
	
	. = 0x10000;
	.text : 
	{ 
		*sys.o(.startup);
		KEEP(*sys.o(.vectors));
		KEEP(*(.startup));
		KEEP(*(.vectors))
		*(.text) 
	}

	. = 0x08000000;
	.data : { *(.data) }
	.bss : { *(.bss) }

}

ENTRY(reset_handler)
