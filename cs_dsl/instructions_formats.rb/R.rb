module SimInfra
    def self.format_r(opcode, funct3, funct7)
        return :R, [
            field(:rd, 11, 7, :reg),
            field(:rs1, 15, 19, :reg),
            field(:rs2, 20, 24, :reg),
            field(:opcode, 0, 6, opcode),
            field(:funct7, 25, 31, funct7),
            field(:funct3, 12, 14, funct3)
        ]
    end

    def self.format_r_alu(name)
        funct3, funct7 =
            {
                add: [0, 0],
                sub: [0, 1 << 5]
                # add more instruction here
            }[name]
        format_r 0, funct3, funct7
    end
end
