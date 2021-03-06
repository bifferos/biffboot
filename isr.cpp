

#include "io.h"
#include "isr.h"
#include "iolib.h"
#include <stdio.h>
#include "idt.h"

isr_t interrupt_handlers[256];

void register_interrupt_handler(u8 n, isr_t handler)
{
    interrupt_handlers[n] = handler;
}

// This gets called from our ASM interrupt handler stub.
extern "C" void isr_handler(registers_t regs)
{
    printf("recieved interrupt: %d\n", regs.int_no);

    if (interrupt_handlers[regs.int_no] != 0)
    {
        isr_t handler = interrupt_handlers[regs.int_no];
        handler(regs);
    }
}

// This gets called from our ASM interrupt handler stub.
extern "C" void irq_handler(registers_t regs)
{
//    printf("recieved irq: ");
//    print_dec32(regs.int_no);
//    printf("\n");
    // Send an EOI (end of interrupt) signal to the PICs.
    // If this interrupt involved the slave.
    if (regs.int_no >= 40)
    {
        //printf("Sending reset signal to slave\n");
        // Send reset signal to slave.
        idt_outb_delay(0x20, 0xA0);
    }
    // Send reset signal to master. (As well as slave, if necessary).
    idt_outb_delay(0x20, 0x20);
    
    // linux seems to do
    //idt_outb_delay(0x60, 0x20);

    if (interrupt_handlers[regs.int_no] != 0)
    {
        isr_t handler = interrupt_handlers[regs.int_no];
        handler(regs);
    }

}
