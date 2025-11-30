require_relative 'yml_parser'
require_relative 'instruction_builder'
require_relative 'constant'
require_relative 'var'
require_relative 'scope'
require_relative 'pc'
require_relative 'utils'
require_relative 'rv32i_decoder_gen'
require_relative 'rv64i_executer_gen'

require 'yaml'

if __FILE__ == $0
    # TODO(mgroshev) move isa_dir in config or .sh
    isa_dir      = "#{__dir__}/../rv64i_isa.yml"
    decoder_dir  = "#{__dir__}/../../src/decode_execute_module/decoder"
    executer_dir = "#{__dir__}/../../src/decode_execute_module/executer"
    instructions_opcode_dir = "#{__dir__}/../../src/decode_execute_module"
    parsed_log_dir = "#{__dir__}/../../build/parse_result_log.txt"
    parsed_isa = SimInfra.parse_isa(isa_dir)
    decoder_generator = DecoderGenerator.new(parsed_isa, isa_dir, decoder_dir)
    decoder_generator.generate_decoder
    executer_generator = ExecuterGenerator.new(SimInfra.instructions, isa_dir, executer_dir, instructions_opcode_dir)
    executer_generator.generate_executer

    SimInfra.siminfra_result(parsed_log_dir)
end