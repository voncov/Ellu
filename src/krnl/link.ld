ENTRY(main)
SECTIONS
{
	. = 0x100000;
	.text ALIGN(4) : {*(.text)}
	.data ALIGN(4) : {*(.data)}
	.rodata ALIGN(4) : {*(.rodata)}
	.bss ALIGN(4) : {*(.bss)}
}