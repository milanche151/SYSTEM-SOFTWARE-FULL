.section text

# Test or instruction
# or regS, regD - regD = regD | regS

# Load test values
ld $0xFF00FF00, %r1
ld $0x00FF00FF, %r2
ld $0xAAAAAAAA, %r3
ld $0x55555555, %r4

# Test OR operation
or %r2, %r1  # r1 = 0xFF00FF00 | 0x00FF00FF = 0xFFFFFFFF
or %r4, %r3  # r3 = 0xAAAAAAAA | 0x55555555 = 0xFFFFFFFF

# Test with zero
ld $0x12345678, %r5
or %r0, %r5  # r5 = 0x12345678 | 0 = 0x12345678

# Test pattern
ld $0x0F0F0F0F, %r6
ld $0xF0F0F0F0, %r7
or %r7, %r6  # r6 = 0x0F0F0F0F | 0xF0F0F0F0 = 0xFFFFFFFF

halt

.section data
or_data: .word 0xCAFEBABE
