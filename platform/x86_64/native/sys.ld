SECTIONS 
{
	. = 1M;

	_TEXT_START = .;
	.text :
	{
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
		_BSS_START = .;
		*(COMMON)
		*(.bss)
	}
	_BSS_END = .;

	. = ALIGN(8);
	_DATA_END = .;

	. = ALIGN(4k);

	_HEAP_START = .;
	_HEAP_SIZE = 0x40000;
}

ENTRY(multiboot_start)
