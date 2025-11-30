#include "mmu.hpp"

// VA  - Virtual Address
// PA  - Physical Address
// PTE - Page Table Entry
// VPN - Virtual Page Number
// PPN - Physical Page Number

// VA Layout (Sv39, 39-bit virtual addresses)
// 63            39 38        30 29        21 20        12 11          0
// +-----------------+-----------+-----------+-----------+--------------+
// |  Sign Extension |  VPN[2]   |  VPN[1]   |  VPN[0]   |  Page Offset |
// |     25 bits     |   9 bits  |   9 bits  |   9 bits  |   12 bits    |

// PA Layout (Sv39, up to 56-bit physical addresses)
//                  49         30 29       21 20       12 11           0
// +---------------+-------------+-----------+-----------+--------------+
// |   Reserved    |   PPN[2]    |  PPN[1]   |  PPN[0]   |  Page Offset |
// |               |  20 bits    |   9 bits  |   9 bits  |   12 bits    |

// Page Table Entry (Sv39, 64-bit PTE)
// 63                   48 47        28 27       19 18       10 9   7   6   5   4      1   0
// +----------------------+------------+-----------+-----------+-----+----+----+--------+----+
// |     Reserved         |  PPN[2]    |  PPN[1]   |  PPN[0]   | RSW | D  | R  | Type   | V  |
// |      16 bits         |  20 bits   |   9 bits  |   9 bits  | 3b  | 1b | 1b | 4 bits | 1b |

TranslateResult MMU::translate(va_t va,
                                 AccessType type,
                                 const HartContext &ctx)
{
    using EC = ExceptionCause;

    // satp.MODE is bits 63:60
    uint64_t mode = (ctx.satp >> 60) & 0xF;
    const uint64_t SV39_MODE = 8;

    if (mode == 0) {
        // Bare mode
        return { .pa = static_cast<pa_t>(va), .e = Exception{EC::None} };
    }
    if (mode != SV39_MODE) {
        return { .pa = 0, .e = Exception{EC::PageFault} };
    }

    uint64_t vpn[3];
    vpn[0] = (va >> 12) & 0x1FF;
    vpn[1] = (va >> 21) & 0x1FF;
    vpn[2] = (va >> 30) & 0x1FF;

    uint64_t page_offset = va & 0xFFF;

    int i = LEVELS - 1;
    uint64_t pte = 0;

    // satp.PPN lower 44 bits - physical page-number of root table
    const uint64_t SATP_PPN_MASK = ((1ull << 44) - 1);
    const pa_t root_table = (ctx.satp & SATP_PPN_MASK) * PAGESIZE; 
    
    uint64_t a = root_table;
    while (true) {
        uint64_t pte_addr = a + vpn[i] * PTESIZE;
        pte = mem_.read(pte_addr, 8);

        uint64_t V = pte & 1;
        uint64_t R = (pte >> 1) & 1;
        uint64_t W = (pte >> 2) & 1;
        uint64_t X = (pte >> 3) & 1;
        // TODO Check Privilige Mode
        // uint64_t U = (pte >> 4) & 1;

        if (V == 0 || (R == 0 && W == 1)) {
            return { .pa = 0, .e = Exception{EC::PageFault} };
        }

        // Check if leaf PTE
        if (R == 1 || X == 1) {
            break;
        }

        i--;
        if (i < 0) {
            return { .pa = 0, .e = Exception{EC::PageFault} };
        }

        uint64_t next_ppn = (pte >> 10);
        a = next_ppn * PAGESIZE;
    }

    uint64_t ppn_all = (pte >> 10);
    uint64_t ppn0 = ppn_all & 0x1FF;
    uint64_t ppn1 = (ppn_all >> 9) & 0x1FF;
    uint64_t ppn2 = (ppn_all >> 18);

    // Apply superpage rules:
    //   If leaf at level i, PPN[j] = VPN[j] for j < i
    uint64_t final_ppn0 = (i > 0) ? vpn[0] : ppn0;
    uint64_t final_ppn1 = (i > 1) ? vpn[1] : ppn1;
    uint64_t final_ppn2 = ppn2;

    uint64_t ppn_combined =
        (final_ppn2 << 18) |
        (final_ppn1 << 9)  |
        (final_ppn0);

    uint64_t pa64 = (ppn_combined << 12) | page_offset;

    return {
        .pa = static_cast<pa_t>(pa64),
        .e  = Exception{EC::None}
    };
}
