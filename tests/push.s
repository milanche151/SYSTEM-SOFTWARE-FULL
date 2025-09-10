.section text

# Test push instruction
# push register onto stack

# Set up stack pointer first
ld $0x20000000, %r14

# Load test values
ld $0x12345678, %r1
ld $0x87654321, %r2
ld $0xAABBCCDD, %r3

# Test pushing different registers
push %r1
push %r2
push %r3

# Push general purpose registers
push %r0

halt

.section data
stack_data: .word 0xDEADBEEF
