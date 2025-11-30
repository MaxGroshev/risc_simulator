require_relative 'rv32i_executer_gen'

class ExecuterGenerator
  alias_method :old_translate_stmt, :translate_stmt

  def translate_stmt(stmt, indent)
    case stmt.name
    when :addw, :subw
      dest, a, b = get_operands(stmt.oprnds)
      a32 = "#{a} & 0xFFFFFFFFULL"
      b32 = "#{b} & 0xFFFFFFFFULL"
      op = stmt.name == :addw ? '+' : '-'
      "#{indent}int32_t tmp = static_cast<int32_t>(#{a32}) #{op} static_cast<int32_t>(#{b32});\n" \
      "#{indent}#{dest} = static_cast<uint64_t>(static_cast<int64_t>(tmp));"

    when :shl_w
      dest, a, b = get_operands(stmt.oprnds)
      "#{indent}uint32_t res = static_cast<uint32_t>(#{a} & 0xFFFFFFFFULL) << #{b};\n" \
      "#{indent}#{dest} = static_cast<uint64_t>(res);"

    when :srl_w
      dest, a, b = get_operands(stmt.oprnds)
      "#{indent}uint32_t res = static_cast<uint32_t>(#{a} & 0xFFFFFFFFULL) >> #{b};\n" \
      "#{indent}#{dest} = static_cast<uint64_t>(res);"

    when :sra_w
      dest, a, b = get_operands(stmt.oprnds)
      "#{indent}int32_t res = static_cast<int32_t>(#{a} & 0xFFFFFFFFULL) >> #{b};\n" \
      "#{indent}#{dest} = static_cast<uint64_t>(static_cast<int64_t>(res));"

    when :load_from_mem
      size = stmt.attrs[:size] || :word
      size_bytes = {byte: 1, half: 2, word: 4, double: 8}[size]
      "#{indent}#{get_var_name(stmt.oprnds[0])} = hart.load(#{get_var_name(stmt.oprnds[1])}, #{size_bytes});"

    when :store_to_mem
      size = stmt.attrs[:size] || :word
      size_bytes = {byte: 1, half: 2, word: 4, double: 8}[size]
      "#{indent}hart.store(#{get_var_name(stmt.oprnds[0])}, #{get_var_name(stmt.oprnds[1])}, #{size_bytes});"

    when :ecall
      "#{indent}hart.do_ecall();"

    else
      old_translate_stmt(stmt, indent)
    end
  end
end