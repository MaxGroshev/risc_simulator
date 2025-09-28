require 'yaml'
require_relative 'instructions_formats.rb/R'

module SimInfra
    def self.assert(condition, msg = nil); raise msg if !condition; end

    @@instructions = [] # kind of static variable inside module
    InstructionInfo = Struct.new(:name, :fields, :frmt, :code)
    class InstructionInfoBuilder
        def initialize(name); @info = InstructionInfo.new(name); end
        def encoding(frmt, fields); @info.fields = fields; @info.frmt = frmt; end
        attr_reader :info

        def code(&block)
            @info.code = scope = Scope.new(nil) # root scope
            dst = nil
            @info.fields.each { |f|
                scope.add_var(f.name, :i32) if [:rs1, :rs2, :rd].include?(f.name)
                # if field has source register we get register
                scope.stmt(:getreg, [f.name, f]) if [:rs1, :rs2].include?(f.name)
                dst = f if [:rd].include?(f.name)
            }
            scope.instance_eval &block
            scope.stmt(:setreg, [dst, dst.name]) if dst
        end
    end

    def self.Instruction(name, &block)
        bldr = InstructionInfoBuilder.new(name)
        bldr.instance_eval &block
        @@instructions << bldr.info
        nil # only for debugging in IRB
    end

    Field = Struct.new(:name, :from, :to, :value)
    ImmFieldPart = Struct.new(:name, :from, :to, :hi, :lo)
    def self.field(name, from, to, value = nil); Field.new(name, from, to, value).freeze; end
    def self.immpart(name, from, to, hi, lo); ImmFieldPart.new(name, from, to, hi, lo).freeze; end
end
