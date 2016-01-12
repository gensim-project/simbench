SECTIONS 
{
	. = 1M;

	.text : 
	{ 
		_TEXT_START = .;
		KEEP(*(.bootstrap))
		*(.text)
		*(.text.*)
	}

	_TEXT_END = .;

	. = ALIGN(4k);

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

	. = ALIGN(4k);

	_HEAP_START = .;
	_HEAP_SIZE = 0x400000;
}

ENTRY(multiboot_start)
