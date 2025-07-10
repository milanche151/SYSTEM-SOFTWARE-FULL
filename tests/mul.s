.section text

# Test mul instruction
# mul regS, regD - regD = regD * regS

# Load test values
ld $10, %r1
ld $5, %r2
ld $0x100, %r3
ld $0x10, %r4

# Test multiplication
mul %r2, %r1  # r1 = 10 * 5 = 50
mul %r4, %r3  # r3 = 0x100 * 0x10 = 0x1000

# Test with zero register
ld $999, %r5
mul %r0, %r5  # r5 = 999 * 0 = 0

# Test large numbers
ld $0x1000, %r6
ld $0x1000, %r7
mul %r7, %r6  # r6 = 0x1000 * 0x1000 = 0x1000000

halt

.section data
mul_data: .word 0xAABBCCDD
