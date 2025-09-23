.section code
ld $0xa, %r1
inc %r1
inc simbol
ld simbol, %r2
halt
simbol:
.word 3
.end