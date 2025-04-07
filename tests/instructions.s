.section text
ld b, %r2
ld 0x12345678, %r3
halt

.section data

a: .word 19
b: .word 1
c: .word a