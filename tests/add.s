.section text

# Test add instruction
# add regS, regD - regD = regD + regS

# Load test values
ld $100, %r1
ld $200, %r2
ld $0x1000, %r3
ld $0x2000, %r4

# Test addition
add %r1, %r2  # r2 = 200 + 100 = 300
add %r3, %r4  # r4 = 0x2000 + 0x1000 = 0x3000

# Test with zero register
add %r0, %r5  # r5 = 0 + 0 = 0

# Test overflow case
ld $0xFFFFFFFF, %r6
ld $1, %r7
add %r7, %r6  # Should overflow

halt

.section data
add_data: .word 0x12345678
