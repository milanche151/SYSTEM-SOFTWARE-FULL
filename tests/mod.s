.section text

.equ term_out, 0xFFFFFF00
.equ tim_cfg, 0xFFFFFF10

main:
    ld $0xFFFFFF00, %sp
    push %sp
    ld $0, %r1
    st %r1, tim_cfg
    ld $handler, %r1
    csrwr %r1, %handler
loop:
    ld done, %r1
    ld $0, %r2
    beq %r1, %r2, loop
    halt

handler:
    push %r1
    push %r2
    push %r3
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
    jmp finish
print:
    st %r2, term_out
    jmp out
finish:
    ld $1, %r1
    st %r1, done
    jmp out
out:
    pop %r3
    pop %r2
    pop %r1
    iret

.section data
counter:
.word 0
done:
.word 0
string:
.ascii "Lokamo"
.word 0
.ascii "Mnogi nisu svesni kolko mi pijemo\n"