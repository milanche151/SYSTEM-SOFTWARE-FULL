.section text
    ld $a, %r1
    ld $c, %r2

.section data
a:
    .word b
b:
    .word 0x19

.global a
.global b
.extern c

.section rodata