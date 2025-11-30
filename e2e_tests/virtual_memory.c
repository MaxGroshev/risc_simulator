// ----------------------------------------------------------
// Minimal RV64 Sv39 virtual memory test (no libc)
// ----------------------------------------------------------
// Expects the following linker-provided symbols:
//
//   extern char _text_start[], _text_end[];
//   extern char _data_start[], _data_end[];
//
// Also expects a region of memory reserved for page tables.
//
// This test:
//   - Builds a simple Sv39 page table structure
//   - Maps text and data 1:1 (VA == PA) but through paging
//   - Enables paging
//   - Jumps to virtual address and verifies mapping
// ----------------------------------------------------------

#include <stdint.h>

#define PTE_V   (1ULL << 0)
#define PTE_R   (1ULL << 1)
#define PTE_W   (1ULL << 2)
#define PTE_X   (1ULL << 3)
#define PTE_U   (1ULL << 4)
#define PTE_G   (1ULL << 5)
#define PTE_A   (1ULL << 6)
#define PTE_D   (1ULL << 7)

#define PAGE_SIZE 4096ULL
#define VPN_MASK  0x1FFULL

// Provide table memory (must be PAGE_SIZE aligned)
static uint64_t l1_page_table[512] __attribute__((aligned(4096)));
static uint64_t l2_page_table[512] __attribute__((aligned(4096)));
static uint64_t l3_page_table[512] __attribute__((aligned(4096)));

// Linker-provided real load addresses
extern char _text_start[], _text_end[];
extern char _data_start[], _data_end[];


// ----------------------------------------------------------
// Write PTE inside a table
// ----------------------------------------------------------
static inline void map_page(uint64_t *l3, uint64_t va, uint64_t pa, uint64_t flags)
{
    uint64_t vpn0 = (va >> 12) & VPN_MASK;
    l3[vpn0] = ((pa >> 12) << 10) | flags;
}


// ----------------------------------------------------------
// Build full Sv39 mapping for text and data ranges
// ----------------------------------------------------------
static void build_sv39_page_tables(void)
{
    // connect L1 -> L2
    uint64_t l1_index = 0; // using VA=0 for simplicity
    l1_page_table[l1_index] =
        ((uint64_t)l2_page_table >> 2) | PTE_V;

    // connect L2 -> L3
    uint64_t l2_index = 0;
    l2_page_table[l2_index] =
        ((uint64_t)l3_page_table >> 2) | PTE_V;

    // Map each 4KiB page of text
    uint64_t ts = (uint64_t)_text_start;
    uint64_t te = (uint64_t)_text_end;

    for (uint64_t p = ts; p < te; p += PAGE_SIZE)
        map_page(l3_page_table, p, p, PTE_V | PTE_R | PTE_X | PTE_A | PTE_D);

    // Map each 4KiB page of data
    uint64_t ds = (uint64_t)_data_start;
    uint64_t de = (uint64_t)_data_end;

    for (uint64_t p = ds; p < de; p += PAGE_SIZE)
        map_page(l3_page_table, p, p, PTE_V | PTE_R | PTE_W | PTE_A | PTE_D);
}


// ----------------------------------------------------------
// Enable Sv39 paging
// ----------------------------------------------------------
static inline void enable_vm(uint64_t root_page_table)
{
    // satp format for RV64 Sv39:
    // MODE(63..60) = 8 (Sv39)
    // ASID(59..44)
    // PPN(43..0)
    uint64_t satp_value =
        (8ULL << 60) |              // Sv39
        ((root_page_table >> 12) & 0xFFFFFFFFFFFULL);

    asm volatile("sfence.vma zero, zero");
    asm volatile("csrw satp, %0" :: "r"(satp_value));
    asm volatile("sfence.vma zero, zero");
}


// ----------------------------------------------------------
// A simple function to call through virtual address
// ----------------------------------------------------------
int test_function()
{
    return 12345;
}


// ----------------------------------------------------------
// _start — entry point
// ----------------------------------------------------------
void _start(void)
{
    build_sv39_page_tables();

    enable_vm((uint64_t)l1_page_table);

    // Call function via virtual address (identity mapped)
    int (*fn)(void) = (int (*)(void))(uint64_t)&test_function;
    int result = fn();

    // Write result into data section to test data mapping via VA
    volatile int *out = (int *)&_data_start[0];
    *out = result;

    // Stop: typically trap into simulator environment
    while (1)
        asm volatile("wfi");
}
