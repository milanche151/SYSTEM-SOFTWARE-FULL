.section text

# Test sub instruction  
# sub regS, regD - regD = regD - regS

# Load test values
ld $300, %r1
ld $100, %r2
ld $0x3000, %r3
ld $0x1000, %r4

# Test subtraction
sub %r2, %r1  # r1 = 300 - 100 = 200
sub %r4, %r3  # r3 = 0x3000 - 0x1000 = 0x2000

# Test with zero register
ld $500, %r5
sub %r0, %r5  # r5 = 500 - 0 = 500

# Test underflow case
ld $10, %r6
ld $20, %r7
sub %r7, %r6  # r6 = 10 - 20 = negative (underflow)

halt

.section data
sub_data: .word 0x87654321
