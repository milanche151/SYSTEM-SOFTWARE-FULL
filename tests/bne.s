.section text

# Test bne instruction (branch if not equal)
# bne reg1, reg2, operand

# Set up stack pointer  
ld $0x20000000, %r14

# Load different values into registers
ld $42, %r1
ld $24, %r2
ld $10, %r3
ld $10, %r4

# Test branch with different values - should branch
bne %r1, %r2, $target1

# Should not execute
ld $0xDEADBEEF, %r5

target1:
    # Test branch with same values - should not branch
    bne %r3, %r4, $target2
    
    # Should execute this
    ld $0xBEEFCAFE, %r6

target2:
    halt

.section data
branch_data: .word 0x12345678
