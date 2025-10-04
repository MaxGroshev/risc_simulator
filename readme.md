#How to build#
```bash
mkdr build
cd ./build
cmake ..
make
```
## Run ruby ##
### 1) run target ###
```
make run_ruby_generation
```
right now this target calls every time, when you try to build decoder or executer

### 2) run mannualy ###
```
ruby ../path_to/isa_dsl/main.rb
```

## Run instruction decoder gtests ##
```bash
./build_dir/decode_execute_module/decoder/tests/decoder_tests
```
please add gtest any time you add new instruction

### ATTENTION ###
P.S output c++ files are hardcoded in .rb(move this logic to cmake or config file)

## EPIC ##

- [x] mgroshev: Implemented parsing of isa all needed instruction for factorial(added all main formats of instructions), further extending should be easier. 
- [x] mgroshev: implemented building of tree to generate cpp handlers of instructions(check out dump during building or during executing of ruby script)
- [x] mgroshev: Implemented autogeneration of decoder
- [ ] implement generation of handlers
- [ ] implement parse of elf
- [ ] implement ...
