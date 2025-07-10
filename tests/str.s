.section text

# Test str instruction - all addressing modes  
# str regS, operand

# Set up stack pointer first
ld $0x20000000, %r14

# First load values into registers to store
ld $0x12345678, %r1
ld $0x87654321, %r2
ld $0xAABBCCDD, %r3
ld $0x11223344, %r4

# Test memory direct literal
str %r1, 0x10000000

# Test memory direct symbol
str %r2, store_location

# Test register indirect
str %r3, [%r14]

# Test register indirect with literal displacement
str %r4, [%r14 + 8]

# Test register indirect with negative displacement
ld $0xDEADBEEF, %r5
str %r5, [%r14 - 4]

# Test storing into different memory locations
ld $0xCAFEBABE, %r6
str %r6, buffer1

ld $0xFEEDFACE, %r7
str %r7, buffer2

halt

.section data
store_location: .word 0
buffer1: .word 0
buffer2: .word 0
test_buffer: .word 0, 0, 0, 0
