.section text

# Test ld instruction - all addressing modes
# ld operand, regD

# Set up stack pointer first
ld $0x20000000, %r14

# Test immediate literal
ld $0x12345678, %r1

# Test immediate symbol
ld $test_symbol, %r2

# Test memory direct literal
ld 0x10000000, %r3

# Test memory direct symbol
ld test_data, %r4

# Test register direct
ld %r1, %r5

# Test register indirect
ld [%r14], %r6

# Test register indirect with negative displacement

halt

.section data
test_symbol: .word 0x87654321
test_data: .word 0xAABBCCDD