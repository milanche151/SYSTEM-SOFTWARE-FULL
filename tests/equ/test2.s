.equ a, b
.equ b, a

.equ c, ext1 - ext2

.extern ext1
.extern ext2

.section text
x:
    .word 10

.equ x, 100