.section text

# Test st instruction - all addressing modes  
# st regS, operand

# Set up stack pointer first
ld $0x20000000, %r14

# First load values into registers to store
ld $0x12345678, %r1
ld $0x87654321, %r2
ld $0xAABBCCDD, %r3
ld $0x11223344, %r4

# Test memory direct literal
st %r1, 0x10000000
ld 0x10000000, %r8

# Test memory direct symbol
st %r2, store_location
ld store_location, %r9

# Test register indirect
st %r3, [%r14]
ld [%r14], %r10

# Test register indirect with literal displacement
st %r4, [%r14 + 8]
ld [%r14+8], %r11

# Test storing into different memory locations
ld $0xCAFEBABE, %r6
st %r6, buffer1
ld buffer1, %r12

ld $0xFEEDFACE, %r7
st %r7, buffer2
ld buffer2, %r13

ld $0xFEEDFACE, %r7


halt

.section data
store_location: .word 0
buffer1: .word 0
buffer2: .word 0
test_buffer: .word 0, 0, 0, 0

