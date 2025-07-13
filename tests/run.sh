AS="build/assembler"
LD="build/linker"
EMU=build/emulator

#$AS tests/file1.s -o tests/file1.o
#$AS tests/file2.s -o tests/file2.o
$AS tests/interrupt.s -o tests/assembled.o

#$LD  tests/file1.o tests/file2.o -o tests/a.hex -hex -place=my_code_main@0x40000000 -place=data@0x10000000 -place=text@0x20000000

$LD  tests/assembled.o -o tests/a.hex -hex -place=text@0x40000000 -place=data@0x20000000

$EMU tests/a.hex