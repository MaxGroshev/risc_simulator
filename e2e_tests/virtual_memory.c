// ----------------------------------------------------------
// Minimal RV64 Sv39 virtual memory test
// ----------------------------------------------------------
// This test:
//   - Builds a simple Sv39 page table structure
//   - Maps text and data 1:1 (VA == PA)
//   - Enables paging
//
// Build: riscv64-linux-gnu-gcc -O1 -o virtual_memory.elf -march=rv64imafd  virtual_memory.c -nostdlib -T simple.ld -fno-pic -fno-pie -static
// ----------------------------------------------------------

typedef unsigned long long int uint64_t;

#define PTE_V   (1ULL << 0)
#define PTE_R   (1ULL << 1)
#define PTE_W   (1ULL << 2)
#define PTE_X   (1ULL << 3)

#define PAGE_SIZE 4096ULL
#define VPN_MASK  0x1FF

uint64_t l1_page_table[512] __attribute__((aligned(PAGE_SIZE)));
uint64_t l2_page_table[512] __attribute__((aligned(PAGE_SIZE)));
uint64_t l3_page_table[512] __attribute__((aligned(PAGE_SIZE)));

// Linker-provided addresses
extern char _text_start[], _text_end[];
extern char _data_start[], _data_end[];
extern char _stack[];

void map_page(uint64_t va, uint64_t flags) {
    // connect L1 -> L2
    uint64_t l1_index = (va >> 30) & VPN_MASK;
    l1_page_table[l1_index] =
        ((uint64_t)l2_page_table >> 2) | PTE_V;

    // connect L2 -> L3
    uint64_t l2_index = (va >> 21) & VPN_MASK;
    l2_page_table[l2_index] =
        ((uint64_t)l3_page_table >> 2) | PTE_V;

    // L3 1:1 mapping
    uint64_t l3_index = (va >> 12) & VPN_MASK;
    l3_page_table[l3_index] = ((va >> 12) << 10) | flags;
}

// ----------------------------------------------------------
// Build full Sv39 mapping for text and data ranges
// ----------------------------------------------------------
void build_sv39_page_tables(void)
{
    // Map each 4KiB page of text
    uint64_t ts = (uint64_t)_text_start;
    uint64_t te = (uint64_t)_text_end;

    for (uint64_t va = ts; va < te; va += PAGE_SIZE) {
        map_page(va, PTE_V | PTE_R | PTE_X);
    }

    // Map each 4KiB page of data
    uint64_t ds = (uint64_t)_data_start;
    uint64_t de = (uint64_t)_data_end;

    for (uint64_t va = ds; va < de; va += PAGE_SIZE) {
        map_page(va,  PTE_V | PTE_R | PTE_W);
    }

    // Map 4KiB page for stack
    uint64_t st = (uint64_t)_stack;
    map_page(st, PTE_V | PTE_R | PTE_W);
}


void enable_vm(uint64_t root_page_table)
{
    // satp format for RV64 Sv39:
    // MODE(63..60) = 8 (Sv39)
    // ASID(59..44)
    // PPN(43..0)
    uint64_t satp_value =
        (8ull << 60) // Sv39
        | ((root_page_table >> 12) & 0xFFFFFFFFFFF);

    asm volatile("csrw satp, %0" :: "r"(satp_value));
}


int test_function()
{
    return 12345;
}


void _start(void)
{
    build_sv39_page_tables();

    enable_vm((uint64_t)l1_page_table);

    int result = test_function();

    asm volatile ("mv a0, %0" : : "r"(result)); 
    register volatile unsigned long a7 asm("a7") = 93;
    asm volatile ("ecall");  /* Currently Halt in our simulator */
}
