# RISC-V Simulator

Instruction-level RISC-V simulator with a generated decoder/executer and an
optional x86-64 JIT backend.

## Highlights
- ELF64 loader with a simple MMU and paging support.
- RV64I ISA description in YAML, codegen via Ruby DSL.
- Interpreter with a basic-block cache.
- x86-64 JIT via asmjit (basic blocks and function-level JIT for small programs).
- Optional modules/callbacks and profiling/PGO helper script.

## Requirements
- CMake 3.21+
- C++17 compiler
- Ruby (for code generation)
- Boost program_options
- asmjit (JIT backend)

asmjit must be available to CMake (`find_package(asmjit REQUIRED)`), either via
system packages or a local build. If you build it yourself, point CMake to its
config directory:
```bash
cmake -S . -B build -Dasmjit_DIR=/path/to/asmjit/lib/cmake/asmjit
```

## Build
```bash
mkdir -p build
cmake -S . -B build
cmake --build build -j$(nproc)
```

The binary is `build/bin/riscv_sim`.

## CMake options
```bash
-DDEBUG_EXECUTION=ON         # verbose per-instruction logging
-DENABLE_MODULES=ON          # build modules support (callbacks)
-DPROFILING=ON               # add -g + frame pointers for perf/valgrind
-DGENERATE_PGO=ON            # build instrumentation for profile generation
-DOPTIMIZE_PGO=ON            # build with existing profiles (mutually exclusive with GENERATE_PGO)
```

Notes:
- `ENABLE_MODULES` only affects availability of `--module` at runtime.
- `GENERATE_PGO` and `OPTIMIZE_PGO` are mutually exclusive.

## Run
```bash
build/bin/riscv_sim --input=./e2e_tests/queens.elf
```

CLI options:
```bash
--input,  -i   path to ELF (required)
--config, -c   config file (default: ./config/configx86.conf)
--module, -m   module name (requires ENABLE_MODULES=ON; incompatible with JIT)
--output, -o   statistic dump file (optional)
```

Examples:
```bash
build/bin/riscv_sim --config=./config/configx86.conf --input=./e2e_tests/queens.elf
```

You can also use the helper script:
```bash
./run_x86.sh --elf ./e2e_tests/queens.elf
./run_x86.sh --no-pgo
./run_x86.sh --generate-pgo
```

## Configuration
The default config file is `config/configx86.conf` (see also `config/config.conf`).
It controls:
- `use_jit` (0/1)
- `bb_cache_size`
- `cached_bb_size`
- `jit_bound`
- `max_cycles`
- `initial_pc`, `initial_reg_val`, `read_delay`

**Modules vs JIT**
- Modules are compile-time (`-DENABLE_MODULES=ON`) and runtime (`--module=...`).
- JIT is controlled by `use_jit` in the config file.
- Modules and JIT are mutually exclusive: if `use_jit=1`, modules are disabled.
  To use modules, set `use_jit=0` in your config.


## RISC-V toolchain (examples)
Build a test ELF with a RISC-V toolchain that matches the ISA in
`isa/rv64i_isa.yml`:
```bash
riscv64-unknown-elf-gcc -o sum.elf sum.c -march=rv64i -mabi=lp64 -nostdlib -T simple.ld
```

If you use a different ISA or ABI, adjust the flags accordingly.

## Repo layout
- `src/`: simulator core (Machine, Hart, MMU) and JIT.
- `config/`: CLI and simulator config parsing.
- `isa/` and `isa/dsl/`: YAML ISA descriptions and Ruby codegen.
- `decode_execute_module/`: generated decoder/executer sources.
- `e2e_tests/`: sample programs and linker script.