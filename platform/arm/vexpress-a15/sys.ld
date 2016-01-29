SECTIONS 
{
	/*
	This needs to have a section offset of 0xf0000, since the high vector location is 0xffff0000.
	Also putting this in the linker script is kind of weird.
	*/

	_VECTORS_RELOCATE = 0x8FFF0000;
	
	. = 0x80000000;
	
	_TEXT_START = . ;
	.text :
	{
		*sys.o(.startup)
		KEEP(*(.startup))
		*(.text)
		
		*(.vectors)
	}
	
	.ctors :
	{
		KEEP (*(SORT(.ctors.*)))
		KEEP (*(.ctors))
		
		
	}
	_TEXT_END = .;	
	
	. = ALIGN(1M);

	_DATA_START = .;

	.rodata : AT(_DATA_START)
	{ 
		*(.rodata) 
	}
	
	.data : ALIGN(4k)
	{ 
		*(.data) 
	}
	. = ALIGN(1M);
	
	_BENCHMARKS_START = .;
	.benchmarks : AT(_BENCHMARKS_START)
	{
		KEEP(*(.benchmarks));
	}
	_BENCHMARKS_END = .;
	
	.bss : ALIGN(4k)
	{ 
		*(COMMON)
		*(.bss) 
	}

	_DATA_END = .;
}

ENTRY(reset_handler)
