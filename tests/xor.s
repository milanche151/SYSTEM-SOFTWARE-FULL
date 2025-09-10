.section text

# Test xor instruction
# xor regS, regD - regD = regD ^ regS

# Load test values
ld $0xFF00FF00, %r1
ld $0x00FF00FF, %r2
ld $0xAAAAAAAA, %r3
ld $0x55555555, %r4

# Test XOR operation
xor %r2, %r1  # r1 = 0xFF00FF00 ^ 0x00FF00FF = 0xFFFFFFFF
xor %r4, %r3  # r3 = 0xAAAAAAAA ^ 0x55555555 = 0xFFFFFFFF

# Test XOR with itself (should be zero)
ld $0x12345678, %r5
xor %r5, %r5  # r5 = 0x12345678 ^ 0x12345678 = 0

# Test XOR with zero
ld $0x87654321, %r6
xor %r0, %r6  # r6 = 0x87654321 ^ 0 = 0x87654321

halt

.section data
xor_data: .word 0xFEEDFACE
