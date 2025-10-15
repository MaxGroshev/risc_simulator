module SimInfra
    class Constant
        attr_reader :scope, :name, :type, :value

        def initialize(scope, name, value);
            @const = value; @scope = scope; @type = :iconst; @value = value
            @scope.stmt(:new_const, [self])
        end

        def let(other); raise "Assign to constant"; end
        def inspect; "#{@name}:#{@type} (#{@scope.object_id}) {=#{@const}}"; end
    end

    # class Var
    #     def +(other); @scope.add(self, other); end
    #     def -(other); @scope.sub(self, other); end
    # end
end
