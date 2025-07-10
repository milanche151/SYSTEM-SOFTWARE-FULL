.section text

# Test pop instruction
# pop from stack to register

# Set up stack pointer first
ld $0x20000000, %r14

# First push some values
ld $0x11111111, %r1
ld $0x22222222, %r2
ld $0x33333333, %r3

push %r1
push %r2
push %r3

# Clear registers
ld $0, %r1
ld $0, %r2
ld $0, %r3

# Test popping into different registers
pop %r5
pop %r6
pop %r7

halt

.section data
pop_data: .word 0xBEEFCAFE
