module SimInfra
    def self.format_j(opcode)
        return :J, [
            field(:rd,  7,  11, :reg),
            # field(:imm12to19, 12, 19, :imm),
            field(:imm, 12, 19, :imm),
            # field(:imm, 20, 20, :imm),
            # field(:imm, 21, 30, :imm),
            # field(:imm, 31, 31, :imm),
            field(:opcode, 0, 6, opcode),
        ]
    end
end
