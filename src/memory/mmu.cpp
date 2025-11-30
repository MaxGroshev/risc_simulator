#include "mmu.hpp"

// VA  - Virtual Address
// PA  - Physical Address
// PTE - Page Table Entry
// VPN - Virtual Page Number
// PPN - Physical Page Number

// 4.3.1 Addressing and Memory Protection
//  see https://five-embeddev.com/riscv-priv-isa-manual/Priv-v1.12/supervisor.html#sec:sv32
//
//  When Sv32 virtual memory mode is selected in the MODE field of the satp register, 
//  supervisor virtual addresses are translated into supervisor physical addresses via a two-level page table. 
//  The 20-bit VPN is translated into a 22-bit physical page number (PPN), while the 12-bit page offset is untranslated. 

// VA Layout
// 31        22 21        12 11          0
// +-----------+-----------+--------------+
// |   VPN[1]  |   VPN[0]  |  Page Offset |
// |  10 bits  |  10 bits  |    12 bits   |

// PA Layout
// 31        22 21        12 11          0
// +-----------+-----------+-------------+
// |   PPN[1]  |   PPN[0]  |  Page Offset |
// |  10 bits  |  10 bits  |    12 bits   |

// PTE Layout
// 31         20 19       10 9        8  7   6   5   4   3   2   1   0
// +------------+-----------+----------+---+---+---+---+---+---+---+---+
// |   PPN[1]   |   PPN[0]  | Reserved | D | A | G | U | X | W | R | V |
// |  12 bits   |  10 bits  |   2 bits | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 |
//

// PPN[1] = upper 12 bits of physical page number
// PPN[0] = lower 10 bits of physical page number
//

inline TranslateResult translate(uint32_t va,
                                 AccessType type,
                                 const HartContext &ctx)
{
    using EC = ExceptionCause;

    const int PAGESIZE = 4096;  // 2^12
    const int PTESIZE  = 4;     // 32-bit PTE
    const int LEVELS   = 2;     // Sv32

    uint32_t mode = (ctx.satp >> 31) & 0x1;
    if (mode == 0) {
        return { .pa = va, .e = Exception{EC::None} };
    }

    // TODO: check privilege mode

    uint32_t vpn[2];
    vpn[0] = (va >> 12) & 0x1FF;  // bits 12..20
    vpn[1] = (va >> 22) & 0x1FF;  // bits 22..30
    uint32_t page_offset = va & 0xFFF;

    int i = LEVELS - 1;
    uint32_t pte = 0;
    uint32_t a = (ctx.satp & 0x003FFFFF) * PAGESIZE;

    while (true) {
        uint32_t pte_addr = a + vpn[i] * PTESIZE;
        pte = mem_.read(pte_addr, 4);

        uint32_t V = pte & 0x1;
        uint32_t R = (pte >> 1) & 0x1;
        uint32_t W = (pte >> 2) & 0x1;
        uint32_t X = (pte >> 3) & 0x1;
        uint32_t U = (pte >> 4) & 0x1;
        
        // TODO: For Now ignoring D and A
        // uint32_t D = (pte >> 7) & 0x1;
        // uint32_t A = (pte >> 6) & 0x1;

        if (V == 0 || (R == 0 && W == 1)) {
            return { .pa = 0, .e = Exception{EC::PageFault} }; // TODO: PageFault Must correspond to access type
        }

        // Leaf check
        if (R == 1 || X == 1) {
            // TODO: Check level or add superpage support
            break;
        }

        i--;
        if (i < 0) {
            return { .pa = 0, .e = Exception{EC::PageFault} };
        }

        uint32_t next_ppn = (pte >> 10);
        a = next_ppn << 12;
    }

    // TODO: check access against R/W/X/U bits

    uint32_t ppn[2];
    ppn[0] = (pte >> 10) & 0x3FF;
    ppn[1] = (pte >> 20) & 0xFFF;

    uint32_t pa = (ppn[1] << 22) | (ppn[0] << 12) | page_offset;

    return { .pa = pa, .e = Exception{EC::None} };
}
