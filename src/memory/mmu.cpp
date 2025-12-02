#include "mmu.hpp"

// VA  - Virtual Address
// PA  - Physical Address
// PTE - Page Table Entry
// VPN - Virtual Page Number
// PPN - Physical Page Number

// VA Layout (Sv39, 39-bit virtual addresses)
// 63            39 38         30 29       21 20       12 11           0
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

// 9 bits mask
constexpr va_t VPN_MASK = 0x1FF; 
constexpr pa_t PPN_MASK = 0x1FF;

TranslateResult MMU::translate(va_t va,
                                 AccessType type,
                                 const HartContext &ctx)
{
    using EC = ExceptionCause;


    const uint64_t SV39_MODE = 8;

    if (ctx.mode == 0) {
        return { .pa = va, .e = Exception{EC::None} };
    }
    if (ctx.mode != SV39_MODE) {
        return { .pa = 0, .e = Exception{EC::PageFault} };
    }

    va_t vpn[3];
    vpn[0] = (va >> 12) & VPN_MASK;
    vpn[1] = (va >> 21) & VPN_MASK;
    vpn[2] = (va >> 30) & VPN_MASK;

    pa_t page_offset = va & (PAGESIZE - 1);
    
    pa_t pte = 0;
    int level = LEVELS - 1;

    for (pa_t base = ctx.root_table; 
            true; 
            base = (pte >> 10) * PAGESIZE) 
        {
        pa_t pte_addr = base + vpn[level] * PTESIZE;
        pte = mem_load(pte_addr, sizeof(pa_t));

        flag_t V = pte & PTE_V;
        flag_t R = pte & PTE_R;
        flag_t W = pte & PTE_W;
        flag_t X = pte & PTE_X;

        // TODO Check Other flags

        if (V == 0 || (R == 0 && W == 1)) {
            return { .pa = 0, .e = Exception{EC::PageFault} };
        }

        // Check if leaf PTE
        if (R == 1 || X == 1) {
            break;
        }

        if (--level < 0) {
            return { .pa = 0, .e = Exception{EC::PageFault} };
        }
    }

    pa_t ppn_all = (pte >> 10);
    pa_t ppn0 =  ppn_all       & PPN_MASK;
    pa_t ppn1 = (ppn_all >> 9) & PPN_MASK;
    pa_t ppn2 = (ppn_all >> 18);

    // Apply superpage rules:
    //   If leaf at level 'level', PPN[j] = VPN[j] for j < level
    pa_t final_ppn0 = (level > 0) ? vpn[0] : ppn0;
    pa_t final_ppn1 = (level > 1) ? vpn[1] : ppn1;
    pa_t final_ppn2 = ppn2;

    pa_t ppn_combined =
        (final_ppn2 << 18) |
        (final_ppn1 << 9)  |
        (final_ppn0);

    pa_t pa64 = (ppn_combined << 12) | page_offset;

    return {
        .pa = pa64,
        .e  = Exception{EC::None}
    };
}




void MMU::map_page(HartContext ctx,
                        va_t va,
                        pa_t pa,
                        flag_t flags)
{
    if (ctx.mode == 0) {
        return;
    }
    if (ctx.mode != 8) {
        abort();
    }
    va_t vpn[3];
    vpn[0] = (va >> 12) & VPN_MASK;
    vpn[1] = (va >> 21) & VPN_MASK;
    vpn[2] = (va >> 30) & VPN_MASK;

    pa_t base = ctx.root_table;

    for (int level = LEVELS - 1; level > 0; --level) {
        pa_t pte_addr = base + vpn[level] * PTESIZE;
        pa_t pte = mem_load(pte_addr, sizeof(pa_t));

        flag_t V = pte & PTE_V;

        if (!V) {
            // Allocate the next-level table
            pa_t new_pt = mem_.alloc_phys_page();
            pa_t new_ppn = new_pt >> 12;

            pa_t new_pte =
                (new_ppn << PTE_PPN_SHIFT) |
                PTE_V;   // non-leaf: A/D/R/W/X = 0

            mem_store(pte_addr, new_pte, sizeof(pa_t));
            base = new_pt;
            continue;
        }

        // Follow next-level table
        pa_t next_ppn = (pte >> 10);
        base = next_ppn * PAGESIZE;
    }

    // Now level 0, place leaf PTE
    pa_t leaf_pte_addr = base + vpn[0] * PTESIZE;
    pa_t leaf_ppn = pa >> 12;

    pa_t leaf_pte =
        (leaf_ppn << PTE_PPN_SHIFT) |
        PTE_V |
        (flags & (PTE_R | PTE_W | PTE_X));

    mem_store(leaf_pte_addr, leaf_pte, sizeof(pa_t));
}
