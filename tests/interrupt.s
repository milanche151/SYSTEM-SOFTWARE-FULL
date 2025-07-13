.section text

main:
    ld $0x20000000, %sp
    ld $handler, %r1
    csrwr %r1, %handler
    int
    div %r0, %r0
    halt

handler:
    push %r1
    push %r2
    csrrd %cause, %r1
    ld $1, %r2
    beq %r1, %r2, hardware_intr
    ld $4, %r2
    beq %r1, %r2, software_intr
    jmp handler_out

software_intr:
    ld $0xdeadbeef, %r10
    jmp handler_out

hardware_intr:
    ld $0xfeedbabe, %r11
    jmp handler_out
    
handler_out:
    pop %r2
    pop %r1
    iret