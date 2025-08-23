.global a 
.section data
a:
.word 1
.type a OBJ
.type b FUNC
.section text
b:
halt
.type c OBJ
.extern c