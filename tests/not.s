.section text

# Test not instruction
# not register - bitwise NOT operation

# Load test values
ld $0x12345678, %r1
ld $0xFFFFFFFF, %r2
ld $0x00000000, %r3
ld $0xAAAAAAAA, %r4

# Test NOT operation on different values
not %r1  # ~0x12345678 = 0xEDCBA987
not %r2  # ~0xFFFFFFFF = 0x00000000  
not %r3  # ~0x00000000 = 0xFFFFFFFF
not %r4  # ~0xAAAAAAAA = 0x55555555

# Test with pattern
ld $0x0F0F0F0F, %r5
not %r5  # ~0x0F0F0F0F = 0xF0F0F0F0

halt

.section data
not_data: .word 0xDEADBEEF
