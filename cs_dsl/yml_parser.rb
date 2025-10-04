module SimInfra
    def self.parse_isa(isa_dir)
        isa_config = YAML.load(File.read(isa_dir))

        isa_config["instructions"].each do |instruction_name, instruction_data|
            if(instruction_data["format"] == 'R')
                inst_sym = instruction_name.to_sym
                opcode = instruction_data["opcode"]
                funct3 = instruction_data["funct3"]
                funct7 = instruction_data["funct7"]

                SimInfra::Instruction(inst_sym) {
                    encoding *SimInfra.format_r(opcode, funct3, funct7)
                    code instruction_data['code']
                }
            elsif(instruction_data["format"] == 'I')
                inst_sym = instruction_name.to_sym
                opcode = instruction_data["opcode"]
                funct3 = instruction_data["funct3"]

                SimInfra::Instruction(inst_sym) {
                    encoding *SimInfra.format_i(opcode, funct3)
                    code instruction_data['code']
                }
            elsif(instruction_data["format"] == 'S')
                inst_sym = instruction_name.to_sym
                opcode = instruction_data["opcode"]
                funct3 = instruction_data["funct3"]

                SimInfra::Instruction(inst_sym) {
                    encoding *SimInfra.format_s(opcode, funct3)
                    code instruction_data['code']
                }
            elsif(instruction_data["format"] == 'J')
                inst_sym = instruction_name.to_sym
                opcode = instruction_data["opcode"]

                SimInfra::Instruction(inst_sym) {
                    encoding *SimInfra.format_j(opcode)
                    code instruction_data['code']
                }
            elsif(instruction_data["format"] == 'B')
                inst_sym = instruction_name.to_sym
                opcode = instruction_data["opcode"]

                SimInfra::Instruction(inst_sym) {
                    encoding *SimInfra.format_j(opcode)
                    code instruction_data['code']
                }
            end
        end
        return isa_config
    end
end
