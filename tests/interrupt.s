.section text

main:
    ld $0x20000000, %sp
    ld $handler, %r1
    csrwr %r1, %handler
    csrrd %status,%r5
    csrwr %r0, %status
    ld $0x02, %r2
    st %r2, 0xFFFFFF10
    csrwr %r5,%status
    int
    #div %r0, %r0   
    ld $0x0fffff0, %r3
    ld $0x00000001,%r4
loop: 
    sub %r4,%r3
    add %r4,%r3
    sub %r4,%r3
    add %r4,%r3
    sub %r4,%r3
    bne %r0, %r3,loop
    halt

handler:
    push %r1
    push %r2
    csrrd %cause, %r1
    ld $1, %r2
    beq %r1, %r2, hardware_intr
    ld $4, %r2
    beq %r1, %r2, software_intr
    ld $2, %r2
    beq %r1, %r2, timer_intr
    jmp handler_out

software_intr:
    ld $0xdeadbeef, %r10
    jmp handler_out

hardware_intr:
    ld $0xfeedbabe, %r11
    jmp handler_out
timer_intr:
    ld $1, %r13
    add %r13, %r12
    jmp handler_out
    
handler_out:
    pop %r2
    pop %r1
    iret