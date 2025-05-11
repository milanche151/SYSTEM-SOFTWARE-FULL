.section text
# ld b, %r2
# ld 0x12345678, %r3
# ld $c, %r3
# ld $0x87654321, %r3
# ld %r7, %r5
# ld [%r9], %r10
# ld [%r8 + 0x128], %r6
# halt
# st %r7, 0x11223344
# st %r8, a
# st %r9, [%r4]
# st %r14, [%r8 + 0xfffffffc]
#push %r9
#pop %r8
#jmp c
#call a
#beq %r1, %r1, b
#bgt %r2, %r3, 1234
#bne %r9, %r5, a

#int
#iret
#ret

csrwr %r1, %status
csrrd %status, %r1

.section data

a: .word 0x123456789
b: .word 7
c: .word a