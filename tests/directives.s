.extern a
.extern b
.section test

c:
  .word 0x12345678
d:
  .word c
e: 
  .ascii "abcdefg"
f: