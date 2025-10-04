module SimInfra
    def self.format_i(opcode, funct3)
        return :I, [
            field(:rd, 7, 11, :reg),
            field(:rs1, 15, 19, :reg),
            field(:imm, 20, 31, :imm),
            field(:opcode, 0, 6, opcode),
            field(:funct3, 12, 14, funct3)
        ]
    end
end
