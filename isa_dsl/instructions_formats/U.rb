module SimInfra
    def self.format_u(opcode)
        return :U, [
            field(:rd, 7, 11, :reg),
            field(:imm, 12, 31, :imm),
            field(:opcode, 0, 6, opcode)
        ]
    end
end
