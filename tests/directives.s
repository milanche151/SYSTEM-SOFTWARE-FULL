.extern a
.extern b

.section text

c:
  .word 0x12345678
d:
  .word c
e: 
  .ascii "abcdefghi"

.section data

f:
  .word a

  .word g

g:
  .ascii "abc"
.end
h: