.section text

# Test ret instruction
# ret - return from subroutine (pops PC from stack)

# Set up stack pointer first
ld $0x20000000, %r14

# Simulate a subroutine call by pushing return address
ld $return_point, %r1
push %r1

# Jump to subroutine
jmp $subroutine

return_point:
    # Execution continues here after ret
    ld $0x12345678, %r5
    halt

subroutine:
    # Do some work in subroutine
    ld $0x12345678, %r2
    ld $0x87654321, %r3
    add %r3, %r2
    
    # Return to caller
    ret

.section data
ret_data: .word 0xCAFEBABE
