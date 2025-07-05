AS="build/assembler"
LD="build/linker"

$AS tests/file1.s -o tests/file1.o
$AS tests/file2.s -o tests/file2.o

$LD tests/file1.o tests/file2.o -o tests/a.hex -hex -place=text@0x40000000 -place=data@0x10000000