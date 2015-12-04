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
		
		KEEP(*(.ctors))
	}

	. = 0x08000000;
	.data : 
	{ 
		*(.data) 
	}
	
	.init_array : 
	{
		_CTORS_START = .;
		*(.init_array)
		_CTORS_END = .;
	}
	
	.bss : 
	{ 
		*(.bss) 
	}

}

ENTRY(reset_handler)
