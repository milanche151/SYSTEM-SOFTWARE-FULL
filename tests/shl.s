.section text

# Test shl instruction (shift left)
# shl regS, regD - regD = regD << regS

# Load test values
ld $0x12345678, %r1
ld $4, %r2
ld $0x00000001, %r3
ld $8, %r4

# Test shift left operations
shl %r2, %r1  # r1 = 0x12345678 << 4 = 0x23456780
shl %r4, %r3  # r3 = 0x00000001 << 8 = 0x00000100

# Test shift by zero
ld $0xABCDEF00, %r5
shl %r0, %r5  # r5 = 0xABCDEF00 << 0 = 0xABCDEF00

# Test large shift
ld $0xFFFFFFFF, %r6
ld $16, %r7
shl %r7, %r6  # r6 = 0xFFFFFFFF << 16 = 0xFFFF0000

halt

.section data
shl_data: .word 0x11111111
