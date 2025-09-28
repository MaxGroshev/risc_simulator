module SimInfra
    IrStmt = Struct.new(:name, :oprnds, :attrs)
    class Var
        attr_reader :scope, :name, :type

        def initialize(scope, name, type)
            @scope = scope; @name = name; @type = type;
        end

        # Syntax "var[]=value" is used to assign variable
        # it's similar to "var[hi:lo]=value" for partial assignment
        # dumps states and disables @scope dump
        def inspect; "#{@name}:#{@type} (#{@scope.object_id})"; end
        
        def []=(other); @scope.stmt(:let, [self, other]); end
        def +(other);   @scope.add(self, other); end
        def -(other);   @scope.sub(self, other); end
    end
end
