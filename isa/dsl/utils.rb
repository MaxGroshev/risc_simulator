# module SimInfra
module SimInfra
    def self.siminfra_result(log_dir, msg = nil)
        return @@instructions if Object.const_defined?(:IRB)

        require 'pp'

        File.open(log_dir, "w") do |f|
            PP.pp(@@instructions, f)
        end
        # puts "\n\n\n#{'*' * 100}\n#{' ' * 30}#{msg}\n\n\n"
        # PP.pp @@instructions
    end

    def siminfra_reset_module_state; @@instructions = []; end

    # module SimInfra::GlobalCounter
    module GlobalCounter
        @@counter = -1
        def next_counter; @@counter += 1; end
    end
end
