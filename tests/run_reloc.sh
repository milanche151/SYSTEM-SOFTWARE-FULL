AS="build/assembler"
LD="build/linker"

$AS tests/reloc1.s -o tests/reloc1.o
$AS tests/reloc2.s -o tests/reloc2.o

$LD  tests/reloc1.o tests/reloc2.o -o tests/reloc.o -relocatable