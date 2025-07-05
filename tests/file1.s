.section text

.extern x

main:
    ld 0xffffff00, %sp
    ld x, %r3
    ld $1, %r1
    ld $2, %r2
    add %r1, %r2
loop:
    ld $1, %r4
    add %r3, %r4
    ld $1, %r4
    sub %r1, %r4
    ld $0, %r5
    bne %r5, %r1, loop
    halt