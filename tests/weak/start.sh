ASSEMBLER=../../build/assembler
LINKER=../../build/linker
EMULATOR=../../build/emulator

${ASSEMBLER} -o main.o main.s
${ASSEMBLER} -o file1.o file1.s
${ASSEMBLER} -o file2.o file2.s
${ASSEMBLER} -o file3.o file3.s
${LINKER} -hex \
  -place=text@0x40000000 \
  -o program.hex \
  main.o file1.o file2.o file3.o
${EMULATOR} program.hex