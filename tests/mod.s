.section text

.equ term_out, 0xFFFFFF00

main:
    ld $handler, %r1
    csrwr %r1, %handler
loop:
    jmp loop
    halt

handler:
    push %r1
    csrrd %cause, %r1
    ld $2, %r2
    beq %r2, %r1, timer
    jmp out
timer:
# load old counter
    ld counter, %r1
# inc counter
    ld counter, %r2
    ld $1, %r3
    add %r3, %r2
    st %r2, counter
# load char
    ld $string, %r2
    add %r1, %r2
    ld [%r2], %r2
    ld $0, %r1
    bne %r1, %r2, print
    jmp out
print:
    st %r2, term_out
out:
    pop %r1
    iret

.section data
counter:
.word 0
string:
.ascii "Mnogi nisu svesni kolko mi pijemo\n"