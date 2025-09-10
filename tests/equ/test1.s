.equ a, b + 2
.equ b, c - 4
.equ c, d + 10

.equ e, ext + data - d + 20

.section data
.word 1, 2, 3, 4
d:
.word 5, 6, 7, 8

.extern ext