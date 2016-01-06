SECTIONS 
{
	. = 1M;

	.text : 
	{ 
		_TEXT_START = .;
		KEEP(*(.bootstrap))
		*(.text) 		
		KEEP (*(SORT(.ctors.*)))
		KEEP (*(.ctors))
	}

	_TEXT_END = .;

	.data : ALIGN(4k)
	{ 
		_DATA_START = .;
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

	. = ALIGN(4k);

	_HEAP_START = .;
	_HEAP_SIZE = 0x400000;
}

ENTRY(multiboot_start)
