../build/assembler mod.s -o mod.o
../build/linker mod.o -o program.hex -place=text@0x40000000 
../build/emulator program.hex