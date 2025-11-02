module SimInfra
    def self.format_r(opcode, funct3, funct7)
        return :R, [
            field(:rd, 7, 11, :reg),
            field(:rs1, 15, 19, :reg),
            field(:rs2, 20, 24, :reg),
            field(:opcode, 0, 6, opcode),
            field(:funct7, 25, 31, funct7),
            field(:funct3, 12, 14, funct3)
        ]
    end
end
