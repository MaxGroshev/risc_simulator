module SimInfra
    IrStmt = Struct.new(:name, :oprnds, :attrs)
    class Var
        attr_reader :scope, :name, :type

        def initialize(scope, name, type)
            @scope = scope; @name = name; @type = type;
        end

        def inspect; "#{@name}:#{@type} (#{@scope.object_id})"; end

        # Syntax "var[]=value" is used to assign variable
        # it's similar to "var[hi:lo]=value" for partial assignment
        # dumps states and disables @scope dump
        def []=(other); @scope.stmt(:let, [self, other]); end
        def !=(other);  @scope.not_eq(self, other); end
        def +(other);   @scope.add(self, other); end
        def -(other);   @scope.sub(self, other); end
        def &(other); @scope.bitand(self, other); end
        def |(other); @scope.bitor(self, other); end
        def ^(other); @scope.bitxor(self, other); end
        def <<(other); @scope.shl(self, other); end
        def >>(other); @scope.srl(self, other); end
        def sra(other); @scope.sra(self, other); end  # For >>>
        def ==(other); @scope.eq(self, other); end
        def !=(other); @scope.neq(self, other); end
        def <(other); @scope.lt(self, other); end
        def >(other); @scope.gt(self, other); end
        def ltu(other); @scope.ltu(self, other); end  # <u
        def gtu(other); @scope.gtu(self, other); end  # >u
        def geu(other); @scope.geu(self, other); end  # >=u
        def >=(other); @scope.ge(self, other); end
    end
end
