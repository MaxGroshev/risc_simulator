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
        def stmt(name, operands, attrs = nil);
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
        def memoryLoad(address, op); 
            var_for_val_in_mem = tmpvar(:i32)
            stmt op, [var_for_val_in_mem]
        end

        def memoryStore(address, src, op)
            stmt op, [address, src]
        end

        # TODO(mgroshev): make singleton Object of PC
        def pcHandler(); 
            var(:pc, :i32)
        end

        def setPc(value, op); 
            stmt op, [value]
        end
        
        # TODO(mgroshev): make class?
        def immHandler(); 
            var(:imm, :i32)
        end 

        def ifHandler(condition, op, &body);
            body_scope = Scope.new(self)
            body_scope.instance_eval &body
            # NOTE(mgroshev): if consists of condition var and body        
            stmt op, [condition, body_scope]
            puts 52
        end

        # arithmetic
        def add(a, b); binOp(a, b, :add); end
        def sub(a, b); binOp(a, b, :sub); end
        def not_eq(a, b); binOp(a, b, :not_eq); end
        # memory
        def memory_ld(address); memoryLoad(address, :load_from_mem) end
        def memory_st(address, src); memoryStore(address, src, :store_to_mem) end
        # pc
        def pc(); pcHandler end
        def set_pc(value); setPc(value, :setpc) end
        # imm
        def imm(); immHandler end
        # if
        def if_expr(condition, &body); ifHandler(condition, :if_expr, &body) end
    end
end
