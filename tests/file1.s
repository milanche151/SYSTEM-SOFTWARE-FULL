.section text

.extern x

stack_pointer:
    .word 0xffffff00
.section my_code_main
main:
    ld stack_pointer, %sp
    ld x, %r3
    ld $1, %r1
    ld $2, %r2
    add %r1, %r2
    ld %r3,%r4
loop:
    sub %r1, %r4
    add %r1, %r6
    add %r2, %r7
    ld $0, %r5
    bne %r5, %r4, loop
    halt