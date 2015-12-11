SECTIONS 
{
	. = 0x0;
	.vectors : 
	{
		*vectors.o;
	}

	. = 0x50000000;
	_TEXT_START = .;
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
	_TEXT_END = .;

	_DATA_START = .;
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
	_DATA_END = .;

}

ENTRY(reset_handler)
