ASSEMBLER=../../build/assembler
LINKER=../../build/linker
EMULATOR=../../build/emulator


${ASSEMBLER} -o inctest.o inctest.s
${LINKER} -hex \
  -place=code@0x40000000 \
  -o program.hex \
  inctest.o 
${EMULATOR} program.hex