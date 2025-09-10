BUILD_DIR=../build

ASSEMBLER=${BUILD_DIR}/assembler
LINKER=${BUILD_DIR}/linker
EMULATOR=${BUILD_DIR}/emulator

${ASSEMBLER} -o main.o main.s
${ASSEMBLER} -o handler.o handler.s
${ASSEMBLER} -o isr_terminal.o isr_terminal.s
${ASSEMBLER} -o isr_timer.o isr_timer.s
${LINKER} -hex \
  -place=my_code@0x40000000 \
  -o program.hex \
  main.o isr_terminal.o isr_timer.o handler.o
${EMULATOR} program.hex