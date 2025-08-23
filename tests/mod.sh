../build/assembler stack.s -o mod.o
../build/linker mod.o -o program.hex -place=text@0x40000000 -place=data@0xf0000000 
../build/emulator program.hex