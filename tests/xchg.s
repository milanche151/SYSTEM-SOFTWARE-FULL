.section text

# Test xchg instruction (exchange register contents)
# xchg regS, regD

# Load test values
ld $0x12345678, %r1
ld $0x87654321, %r2
ld $0xAABBCCDD, %r3
ld $0x11223344, %r4

# Test exchange between different register pairs
xchg %r1, %r2
xchg %r3, %r4
xchg %r1, %r3
xchg %r2, %r4

# Exchange with register 0 (should put 0 in other register)
xchg %r0, %r5

halt

.section data
xchg_data: .word 0xDEADBEEF
