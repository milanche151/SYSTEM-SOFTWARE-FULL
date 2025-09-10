.section text

# Test shr instruction (shift right)  
# shr regS, regD - regD = regD >> regS

# Load test values
ld $0x12345678, %r1
ld $4, %r2
ld $0x80000000, %r3
ld $8, %r4

# Test shift right operations
shr %r2, %r1  # r1 = 0x12345678 >> 4 = 0x01234567
shr %r4, %r3  # r3 = 0x80000000 >> 8 = 0x00800000

# Test shift by zero
ld $0xABCDEF00, %r5
shr %r0, %r5  # r5 = 0xABCDEF00 >> 0 = 0xABCDEF00

# Test large shift
ld $0xFFFFFFFF, %r6
ld $16, %r7
shr %r7, %r6  # r6 = 0xFFFFFFFF >> 16 = 0x0000FFFF

halt

.section data
shr_data: .word 0x22222222
