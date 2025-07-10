.section text

# Test call instruction - all addressing modes
# call operand loads PC into register and jumps

# Set up stack pointer first
ld $0x20000000, %r14

# Test immediate literal
call $0x40001000

# Test immediate symbol  
call $main_func

halt

.section data
main_func: .word 0x40001100
