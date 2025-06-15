.section text
f:
    ld $1, %r2
    ret

.section data
c:
    .word 0x12345678

.global f
.global c