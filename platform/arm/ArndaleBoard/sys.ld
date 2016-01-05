SECTIONS 
{
	/*
	This needs to have a section offset of 0xf0000, since the high vector location is 0xffff0000.
	Also putting this in the linker script is kind of weird.
	*/

	_VECTORS_RELOCATE = 0x6F0F0000;

	. = 0x60000000;
	_TEXT_START = .;
	.text : 
	{ 
		*sys.o(.startup);
		KEEP(*(.startup));
		*(.text) 		
		
	*(.vectors)
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
