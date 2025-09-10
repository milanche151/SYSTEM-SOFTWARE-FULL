.section text

# Test beq instruction (branch if equal)
# beq reg1, reg2, operand

# Set up stack pointer
ld $0x20000000, %r14

# Load same values into registers
ld $42, %r1
ld $42, %r2
ld $10, %r3
ld $20, %r4

# Test branch with immediate symbol (should branch)
beq %r1, %r2, $branch_target

# Should not execute this
ld $0xDEADBEEF, %r5

branch_target:
    # Test branch with different values (should not branch)
    beq %r3, %r4, $no_branch

    # Should execute this
    ld $0xBEEFCAFE, %r6

no_branch:
    halt

.section data
branch_data: .word 42
