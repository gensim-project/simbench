PHDRS {
	vector PT_LOAD ;
	text PT_LOAD ;
	data PT_LOAD ;
}

_HEAP_SIZE = 1024*1024*1024 ;

SECTIONS 
{
	. = 0x40000000 ;
	_TEXT_START = .;
	.text : 
	{		
		KEEP(*(.vectors))
		*sys.o(.startup);
		KEEP(*(.startup));
		*(.text)
	} :text
	. = . + SIZEOF(.text);
	
	.ctors :
	{
		KEEP (*(SORT(.ctors.*)))
		KEEP (*(.ctors))
	} :text
	. = . + SIZEOF(.ctors);
	
	_TEXT_END = .;

	. = ALIGN(4k) ;
	_DATA_START = .;

	.rodata : 
	{ 
		*(.rodata) 
	} :data
	. = . + SIZEOF(.rodata);

	. = ALIGN(4k);
	.data : 
	{ 
		*(.data) 
	}  :data 
	. = . + SIZEOF(.data);

	. = ALIGN(4k);
	.benchmarks : 
	{
		. = ALIGN(8);
		_BENCHMARKS_START = .;
		KEEP(*(.benchmarks));
		_BENCHMARKS_END = .;
	}  :data
	. = . + SIZEOF(.benchmarks);
	
	. = ALIGN(4k);
	.bss : 
	{ 
		*(COMMON)
		*(.bss) 
	}  :data
	. = . + SIZEOF(.bss);

	el0_stack_bottom = .;
	. = . + 0x10000;
	el0_stack_top = .;
	el1_stack_bottom = .;
	. = . + 0x10000;
	el1_stack_top = .;
	
	_DATA_END = .;

	. = ALIGN(1M) ;
	_HEAP_START = . ;
	. += _HEAP_SIZE ;
	_HEAP_END = . ;
}

ENTRY(reset_handler)
