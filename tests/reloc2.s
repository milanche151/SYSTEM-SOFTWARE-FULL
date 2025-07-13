.section text
    ld $c, %r3
    ld $d, %r4

.section data
c:
    .word 8

.global c
.extern d