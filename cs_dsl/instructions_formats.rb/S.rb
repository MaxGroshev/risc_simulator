module SimInfra
    def self.format_s(opcode, funct3)
        return :S, [
            field(:rs1, 15, 19, :reg),
            field(:rs2, 20, 24, :reg),
            # NOTE(mgroshev): As we have 2 parts of imm => 2 from and 2 to
            field(:imm, [7, 25], [11, 31], :imm),
            field(:opcode, 0, 6, opcode),
            field(:funct3, 12, 14, funct3)
        ]
    end
end
