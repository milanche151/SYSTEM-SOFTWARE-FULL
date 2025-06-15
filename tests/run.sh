AS="build/assembler"
LD="build/linker"

$AS tests/file1.s -o tests/file1.o
$AS tests/file2.s -o tests/file2.o

$LD