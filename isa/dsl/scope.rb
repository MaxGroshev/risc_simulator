require_relative 'utils'
require_relative 'instruction_builder'

module SimInfra
    class Scope
        include GlobalCounter # used for temp variables IDs
        attr_reader :tree, :vars, :parent

        def initialize(parent); @tree = []; @vars = {}; end

        def var(name, type)
            @vars[name] = SimInfra::Var.new(self, name, type) # return var
            instance_eval "def #{name.to_s}(); return @vars[:#{name.to_s}]; end"
            stmt :new_var, [@vars[name]] # returns @vars[name]
        end

        # wrapper over var to run x.add_var(..).add_var(..)...
        def add_var(name, type); var(name, type); self; end

        private def tmpvar(type); var("_tmp#{next_counter}".to_sym, type); end
        def stmt(name, operands, attrs = nil)
            operands = operands.map do |o|
                if o.is_a?(Numeric)
                    resolve_const(o)
                else
                    o
                end
            end
            @tree << IrStmt.new(name, operands, attrs); operands[0];
        end

        def resolve_const(what)
            return what if (what.instance_of?(Var) or what.instance_of?(Constant)) # or other known classes
            return Constant.new(self, "const_#{next_counter}", what) if (what.instance_of?(Integer))
        end

        def binOp(a, b, op);
            a = resolve_const(a); b = resolve_const(b)
            # TODO: check constant size <= bitsize(var)
            SimInfra.assert(a.type == b.type || a.type == :iconst || b.type == :iconst)

            stmt op, [tmpvar(a.type), a, b]
        end

        # NOTE(mgroshev): other is address in memory
        def memoryLoad(address, op, size=nil, sign=nil)
            var_for_val_in_mem = tmpvar(:i32)
            operands = [var_for_val_in_mem, address]
            attrs = {size: size, sign: sign} if size || sign 
            stmt op, operands, attrs
        end
        
        def memoryStore(address, src, op, size=nil)
            operands = [address, src]
            attrs = {size: size} if size
            stmt op, operands, attrs
        end

        # TODO(mgroshev): make singleton Object of PC
        def pcHandler(); 
            unless @vars[:pc]
                pc = var(:pc, :i32)
                stmt(:getpc, [pc])
            end
            @vars[:pc]
        end

        def setPc(value, op); 
            stmt op, [resolve_const(value)]
        end
        
        # TODO(mgroshev): make class?
        def immHandler(); 
            unless @vars[:imm]
                imm = var(:imm, :i32)
                stmt(:getimm, [imm])
            end
            @vars[:imm]
        end

        def ifHandler(condition, op, &body);
            body_scope = Scope.new(self)
            body_scope.instance_eval &body
            # NOTE(mgroshev): if consists of condition var and body        
            stmt op, [condition, body_scope]
        end

        # TODO: stmt call creates load and store to rs1 / rd what is not needed now and generating wrong code
        # (fixed now with DecodedInstruction zeroing all the fields)
        def ecall
            stmt(:ecall, [])
        end

        def ebreak
            stmt(:ebreak, [])
        end

        # arithmetic
        def add(a, b); binOp(a, b, :add); end
        def sub(a, b); binOp(a, b, :sub); end
        def not_eq(a, b); binOp(a, b, :not_eq); end

        # logical
        def bitand(a, b); binOp(a, b, :bitand); end
        def bitor(a, b); binOp(a, b, :bitor); end
        def bitxor(a, b); binOp(a, b, :bitxor); end
        def shl(a, b); binOp(a, b, :shl); end
        def srl(a, b); binOp(a, b, :srl); end
        def sra(a, b); binOp(a, b, :sra); end
        def eq(a, b); binOp(a, b, :eq); end
        def neq(a, b); binOp(a, b, :neq); end
        def lt(a, b); binOp(a, b, :lt); end
        def gt(a, b); binOp(a, b, :gt); end
        def ltu(a, b); binOp(a, b, :ltu); end
        def gtu(a, b); binOp(a, b, :gtu); end
        def geu(a, b); neq(ltu(a, b), 1); end  # >=u as !(a <u b)
        def ge(a, b); neq(lt(a, b), 1); end

        # memory
        def memory_ld(address, size=:word, sign=:signed); memoryLoad(address, :load_from_mem, size, sign) end
        def memory_st(address, src, size=:word); memoryStore(address, src, :store_to_mem, size) end
        # pc
        def pc(); pcHandler end
        def set_pc(value); setPc(value, :setpc) end
        # imm
        def imm(); immHandler end
        # if
        def if_expr(condition, &body); ifHandler(condition, :if_expr, &body) end

        def addw(a, b)
            stmt(:addw, [tmpvar(:i32), resolve_const(a), resolve_const(b)])
        end

        def subw(a, b)
            stmt(:subw, [tmpvar(:i32), resolve_const(a), resolve_const(b)])
        end

        def shl_w(a, b)
            stmt(:shl_w, [tmpvar(:i32), resolve_const(a), resolve_const(b & 0x1F)])
        end

        def srl_w(a, b)
            stmt(:srl_w, [tmpvar(:i32), resolve_const(a), resolve_const(b & 0x1F)])
        end

        def sra_w(a, b)
            stmt(:sra_w, [tmpvar(:i32), resolve_const(a), resolve_const(b & 0x1F)])
        end
    end
end