OUTPUT_ARCH( "riscv" )
ENTRY(_start)

SECTIONS {
    . = 0x1000;

    _text_start = .;
    .text : { *(.text) }
    _text_end = .;

    _data_start = .;
    .data : { *(.data) }
    _data_end = .;

    _bss_start = .;
    .bss : { *(.bss) }
    _bss_end = .;

    _stack = 0x1000000 - 0x1000;
}