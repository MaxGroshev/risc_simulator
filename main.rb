require_relative 'cs_dsl/yml_parser'
require_relative 'cs_dsl/instruction_builder'
require_relative 'cs_dsl/constant'
require_relative 'cs_dsl/var'
require_relative 'cs_dsl/scope'
require_relative 'cs_dsl/utils'

require 'yaml'

if __FILE__ == $0
    # TODO(mgroshev) move isa_dir in config or .sh
    isa_dir = "#{__dir__}/rv32i_config.yml"

    SimInfra.parse_isa(isa_dir)
    SimInfra.siminfra_result
end
