require_relative 'yml_parser'
require_relative 'instruction_builder'
require_relative 'constant'
require_relative 'var'
require_relative 'scope'
require_relative 'pc'
require_relative 'utils'
require_relative 'decoder_generator'

require 'yaml'

if __FILE__ == $0
    # TODO(mgroshev) move isa_dir in config or .sh
    isa_dir      = "#{__dir__}/../isas/rv32i_isa.yml"
    decoder_dir  = "#{__dir__}/../decode_execute_module/decoder/generated"
    handlers_dir = "#{__dir__}/../decode_execute_module/executer/generated"

    parsed_isa = SimInfra.parse_isa(isa_dir)
    generator = DecoderGenerator.new(parsed_isa, isa_dir, decoder_dir)
    SimInfra.siminfra_result
    generator.generate_decoder
    # puts SimInfra.instructions.length
end
