require_relative 'yml_parser'
require_relative 'instruction_builder'
require_relative 'constant'
require_relative 'var'
require_relative 'scope'
require_relative 'utils'
require_relative 'decoder_generator'

require 'yaml'

if __FILE__ == $0
    # TODO(mgroshev) move isa_dir in config or .sh
    isa_dir = "#{__dir__}/../isas/rv32i_isa.yml"

    SimInfra.parse_isa(isa_dir)
    # puts SimInfra.instructions.length
    # instr = SimInfra.instructions
    generator = DecoderGenerator.new(isa_dir)
    generator.generate_decoder
    SimInfra.siminfra_result
end
