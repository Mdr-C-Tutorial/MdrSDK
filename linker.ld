ENTRY(_start)

SECTIONS {
    . = 0xb0000010;
    .text :
    {
    	*(.text)
    }

    .data ALIGN(0x1000):
    {
    	*(.data)
    }

    .bss :
    {
    	*(COMMON)
    	*(.bss)
    }
}