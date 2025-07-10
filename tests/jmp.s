.section text

# Test jmp instruction - all addressing modes

# Set up stack pointer
ld $0x20000000, %r14

# Test immediate literal
jmp $target_addr

halt

target_addr:
    # Test immediate symbol
    jmp $0x40001000

.section data
jump_target: .word 0x40001200
