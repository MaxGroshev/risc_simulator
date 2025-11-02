module SimInfra
    def self.format_b(opcode, funct3)
        return :B, [
            field(:rs1, 15, 19, :reg),
            field(:rs2, 20, 24, :reg),
            field(:imm7, 7, 7, :imm),
            field(:imm8to11, 8, 11, :imm),
            field(:imm25to30, 27, 30, :imm),
            field(:imm31, 31, 31, :imm),
            field(:opcode, 0, 6, opcode),
            field(:funct3, 12, 14, funct3)
        ]
    end
end
