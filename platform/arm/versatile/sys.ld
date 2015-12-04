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

	.ctors :
	{
		KEEP (*(SORT(.ctors.*)))
		KEEP (*(.ctors))
	}

	. = 0x08000000;
	.data : 
	{ 
		*(.data) 
		SORT(CONSTRUCTORS)
	}
	
	.init_array : 
	{
		_CTORS_START = .;
		KEEP(*(.init_array));
		_CTORS_END = .;
	}
	
	.bss : 
	{ 
		*(.bss) 
	}

}

ENTRY(reset_handler)
