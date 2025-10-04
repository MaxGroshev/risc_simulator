// #include "decoder.hpp"
#include "decoder/generated/rv32i_decoder.hpp"
#include <cstdint>
#include <iostream>

int main() {
    uint32_t instructions[] = {
        0x003100b3, // add x1, x2, x3
        0x00408293, // addi x5, x1, 4
        0x00512423, // sw x5, 8(x2)
        0xff5ff06f  // jal x0, -12
    };
    
    for (int i = 0; i < 4; i++) {
        DecodedInstruction instr = RV32IDecoder::decode(instructions[i]);
        std::string disasm = RV32IDecoder::disassemble(instr);
        
        std::cout << "0x" << std::hex << instructions[i] << " -> " << disasm << std::endl;
    }
    
    return 0;
}