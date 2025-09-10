.section text

# Test iret instruction  
# iret - return from interrupt (pops status and PC from stack)

# Set up stack pointer first
ld $0x20000000, %r14

# Simulate interrupt context by setting up stack
# Push status register (simulate interrupt entry)
csrrd %status, %r1
push %r1

# Push return address  
ld $interrupt_return, %r2
push %r2

# Jump to interrupt handler
jmp $interrupt_handler

interrupt_return:
    # Execution continues here after iret
    ld $0x12345678, %r5
    halt

interrupt_handler:
    # Simulate interrupt handler work
    ld $0x11111111, %r3
    ld $0x22222222, %r4
    
    # Save some registers
    push %r3
    push %r4
    
    # Do interrupt work
    ld $0x33333333, %r6
    
    # Restore registers
    pop %r4
    pop %r3
    
    # Return from interrupt
    iret

.section data
iret_data: .word 0xDEADBEEF
