#include <stddef.h>
#include <stdint.h>
#include "../libs/scheduler.h"
#include "../libs/fork.h"
#include "../libs/utils.h"
#include "../drivers/uart/uart.h"
#include "../drivers/timer/timer.h"
#include "../drivers/irq/controller.h"
#include "../drivers/sd/sd.h"

void process(char* array);

void kernel_main(uint64_t dtb_ptr32, uint64_t x1, uint64_t x2, uint64_t x3)
{
    uart_init();
    uart_puts("Hello, kernel world!\r\n");

    uart_puts("Exception level: ");
    uart_putc('0' + get_el());
    uart_puts("\n");

    irq_vector_init();
    timer_init();
    enable_interrupt_controller();
    enable_irq();

    // EMMC non funziona su qemu
    /*int sd_ok = sd_init();
    if (sd_ok == SD_OK) {
        uart_puts("SD card initialized OK\n");
    } else {
        uart_puts("SD card initialization FAILED\n");
    }
*/
    int error = fork((unsigned long)&process, (unsigned long)"1");
    if (error) {
        uart_puts("Error creating first process\n");
    }
    int error1 = fork((unsigned long)&process, (unsigned long)"2");
    if (error1) {
        uart_puts("Error creating second process\n");
    }

    while (1) {
        schedule();
    }
}

void process(char* array) {
    while (1) {
        uart_puts("Sono il processo ");
        uart_puts(array);
        uart_puts("\n");
    }
}
