require_relative 'cs_dsl/instruction_builder'
require_relative 'cs_dsl/constant'
require_relative 'cs_dsl/var'
require_relative 'cs_dsl/scope'
require_relative 'cs_dsl/utils'

SimInfra::Instruction(:sub) {
    encoding *SimInfra.format_r_alu(:sub)
    code { rd[] = rs1 - rs2 }
}
SimInfra::Instruction(:add) {
    encoding *SimInfra.format_r_alu(:add)
    code { rd[] = rs1 + rs2 }
}
SimInfra.siminfra_result
