.section text

# Test csrwr instruction (CSR write)
# csrwr gprReg, csrReg

# Load test values into general purpose registers
ld $0x12345678, %r1
ld $0x87654321, %r2
ld $0xAABBCCDD, %r3

# Test writing to different CSR registers
csrwr %r1, %status    # Write r1 to status register
csrwr %r2, %handler   # Write r2 to handler register
csrwr %r3, %cause     # Write r3 to cause register

# Test writing different values to same CSR
ld $0x11111111, %r4
ld $0x22222222, %r5
csrwr %r4, %status
csrwr %r5, %status

# Test writing zero
csrwr %r0, %handler

halt

.section data
csrwr_data: .word 0xDEADBEEF
