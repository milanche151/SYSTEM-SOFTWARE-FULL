.section text

# Test csrrd instruction (CSR read)
# csrrd csrReg, gprReg

# Test reading from different CSR registers
csrrd %status, %r1    # Read status register to r1
csrrd %handler, %r2   # Read handler register to r2  
csrrd %cause, %r3     # Read cause register to r3

# Read same CSR to different GPRs
csrrd %status, %r4
csrrd %status, %r5

# Read to different general purpose registers
csrrd %handler, %r6
csrrd %handler, %r7
csrrd %cause, %r8

halt

.section data
csr_data: .word 0x12345678
