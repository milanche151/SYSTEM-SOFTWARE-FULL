.section text

# Test div instruction
# div regS, regD - regD = regD / regS

# Load test values
ld $100, %r1
ld $10, %r2
ld $0x1000, %r3
ld $0x10, %r4

# Test division
div %r2, %r1  # r1 = 100 / 10 = 10
div %r4, %r3  # r3 = 0x1000 / 0x10 = 0x100

# Test division with remainder
ld $23, %r5
ld $5, %r6
div %r6, %r5  # r5 = 23 / 5 = 4

# Test large number division
ld $0x10000, %r7
ld $0x100, %r8
div %r8, %r7  # r7 = 0x10000 / 0x100 = 0x100

halt

.section data
div_data: .word 0x11223344
