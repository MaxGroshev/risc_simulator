#How to build#
```bash
#install dependencies
sudo apt-get install ruby-full

mkdir build
cd ./build
cmake ..
make
```

## RiscV compilation
TODO: Update to rv64
Toolchain:
```bash
mkdir riscv && cd riscv
git clone --recursive --depth=1 https://github.com/riscv/riscv-gnu-toolchain 
cd riscv-gnu-toolchain
mkdir build && cd build
../configure --prefix=/opt/riscv --with-arch=rv32i --with-abi=ilp32 --with-newlib  # RV32I
make -j$(nproc)  
```

Compilation (inside `e2e_tests`):
```bash
riscv32-unknown-elf-gcc -o sum.elf sum.c -march=rv32i -mabi=ilp32 -nostdlib -T simple.ld
```

## Run
```bash
build/bin/decode_execute ../e2e_tests/sum.elf
```
Output will be in register a0 (x10), simulator will print it in console

### ATTENTION ###
P.S output c++ files are hardcoded in .rb(move this logic to cmake or config file)

## EPIC ##

- [x] mgroshev: Implemented parsing of isa all needed instruction for factorial(added all main formats of instructions), further extending should be easier. 
- [x] mgroshev: implemented building of tree to generate cpp handlers of instructions(check out dump during building or during executing of ruby script)
- [x] mgroshev: Implemented autogeneration of decoder
- [ ] implement generation of handlers
- [ ] implement parse of elf
- [ ] implement ...
