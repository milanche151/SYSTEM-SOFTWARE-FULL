.section text

# Test and instruction
# and regS, regD - regD = regD & regS

# Load test values
ld $0xFF00FF00, %r1
ld $0x0F0F0F0F, %r2
ld $0xAAAAAAAA, %r3
ld $0x55555555, %r4

# Test AND operation
and %r2, %r1  # r1 = 0xFF00FF00 & 0x0F0F0F0F = 0x0F000F00
and %r4, %r3  # r3 = 0xAAAAAAAA & 0x55555555 = 0x00000000

# Test with all ones
ld $0xFFFFFFFF, %r5
ld $0x12345678, %r6
and %r5, %r6  # r6 = 0x12345678 & 0xFFFFFFFF = 0x12345678

# Test with zero
and %r0, %r7  # r7 = r7 & 0 = 0

halt

.section data
and_data: .word 0xBEEFCAFE
