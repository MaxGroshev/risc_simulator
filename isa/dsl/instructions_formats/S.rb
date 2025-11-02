module SimInfra
    def self.format_s(opcode, funct3)
        return :S, [
            field(:rs1, 15, 19, :reg),
            field(:rs2, 20, 24, :reg),
            field(:imm7to11, 7, 11, :imm),
            field(:imm25to31, 25, 31, :imm),
            field(:opcode, 0, 6, opcode),
            field(:funct3, 12, 14, funct3)
        ]
    end
end
