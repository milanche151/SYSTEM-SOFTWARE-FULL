.section text
main:
    ld $0x20000000, %sp
    #ld $0x20000020, %sp
    #ld $0x1fffff00, %sp
    ld $1, %r1
    push %r1
    halt
