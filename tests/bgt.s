.section text

# Test bgt instruction (branch if greater than)
# bgt reg1, reg2, operand

# Set up stack pointer
ld $0x20000000, %r14

# Load values for comparison
ld $100, %r1
ld $50, %r2
ld $30, %r3
ld $60, %r4

# Test branch - should branch (100 > 50)
bgt %r1, %r2, $target1

# Should not execute
ld $0xDEADBEEF, %r5

target1:
    # Test branch - should not branch (30 < 60)
    bgt %r3, %r4, $target2
    
    # Should execute this
    ld $0xBEEFCAFE, %r6

target2:
    halt

.section data
compare_data: .word 0x87654321
