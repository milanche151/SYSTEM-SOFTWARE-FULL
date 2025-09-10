.extern a
.extern b

.section text
  add %r1, %r2

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
