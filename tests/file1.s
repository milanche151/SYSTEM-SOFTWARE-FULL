.extern f

.section text
main:
    ld $0, %r1
    call f
    ret

.extern c

.section data
a:
    .word 4
b:
    .word c