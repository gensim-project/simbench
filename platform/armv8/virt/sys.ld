SECTIONS 
{
	
	_VECTORS_RELOCATE = 0x5f0000;
	
	. = 0x0;
	
	_TEXT_START = .;
	.text : 
	{ 
		*(.vectors)
		
		*sys.o(.startup);
		KEEP(*(.startup));
		*(.text) 		
		
	}

	.ctors :
	{
		KEEP (*(SORT(.ctors.*)))
		KEEP (*(.ctors))
	}
	
	_TEXT_END = .;

	. = 0x50000000;
	_DATA_START = .;

	.rodata : ALIGN(4k)
	{ 
		*(.rodata) 
	}

	.data : ALIGN(4k)
	{ 
		*(.data) 
	}

	.benchmarks : ALIGN(4k)
	{
		. = ALIGN(8);
		_BENCHMARKS_START = .;
		KEEP(*(.benchmarks));
		_BENCHMARKS_END = .;
	}
	
	.bss : ALIGN(4k)
	{ 
		*(COMMON)
		*(.bss) 
	}

	_DATA_END = .;

}

ENTRY(reset_handler)
