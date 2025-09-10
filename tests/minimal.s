.section text

ld $0xdeadbeef, %r1
st %r1, 0x10000000
ld 0x10000000, %r8
halt