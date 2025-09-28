module SimInfra
    def self.parse_isa(isa_dir)
        isa_config = YAML.load(File.read(isa_dir))

        isa_config["instructions"].each do |instruction_name, instruction_data|
            # if(format type)
            inst_sym = instruction_data["name"].to_sym
            opcode = instruction_data["opcode"]
            funct3 = instruction_data["funct3"]
            funct7 = instruction_data["funct7"]

            SimInfra::Instruction(inst_sym) {
                encoding *SimInfra.format_r(opcode, funct3, funct7)
                code instruction_data['code']
            }
        end
    end
end
