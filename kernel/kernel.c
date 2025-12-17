#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "../libs/scheduler.h"
#include "../libs/fork.h"
#include "../libs/utils.h"
#include "../libs/syscalls.h"
#include "../drivers/uart/uart.h"
#include "../drivers/timer/timer.h"
#include "../drivers/irq/controller.h"
#include "../drivers/sd/sd.h"
#include "../drivers/sd/sd_filesystem.h"


void kernel_process();
void user_process();
void user_process_fs();
void user_process_print(char* process_name);
void shell();

void kernel_main(uint64_t dtb_ptr32, uint64_t x1, uint64_t x2, uint64_t x3)
{
    uart_init();
    uart_puts("Hello, kernel world!\r\n");

    irq_vector_init();
    timer_init();
    enable_interrupt_controller();
    enable_irq();

    int fs_ok = sd_filesystem_init();
    if (fs_ok == SD_FILESYSTEM_INIT_OK) {
        uart_puts("[DEBUG] SD filesystem init successful.\n");
    } else {
        uart_puts("[DEBUG] SD filesystem init error.\n");
    }

    int res = fork(PF_KTHREAD, (unsigned long)&kernel_process, 0, 0);
}

void kernel_process() {
    uart_puts("Kernel process started.\n");

    int error = move_to_user_mode((unsigned long)&shell);
    if (error < 0) {
        uart_puts("[ERROR] Cannot move process from kernel mode to user mode\n");
    }
}

void shell() {
    char current_path[40] = "/\0";
    call_syscall_write("[SHELL] Welcome to the shell!\n");
    while (1) {
        call_syscall_write("> ");
        char buffer[64] = {0};
        call_syscall_input(buffer, 64);
        call_syscall_write("\n");

        if (memcmp(buffer, "help", 4) == 0) {
            call_syscall_write("[demoos shell - 0.0.1]\n");
        } else if (memcmp(buffer, "ls", 2) == 0) {
            int fd = syscall_open_dir(current_path);
            if (fd == -1) {
                call_syscall_write("[SHELL] Error opening folder '");
                call_syscall_write(buffer);
                call_syscall_write("'.\n");
            }
            call_syscall_write("[SHELL] Folder open with FD '");
            uart_hex(fd);
            call_syscall_write("'.\n");

            FatEntryInfo* info;
            while (1) {
                int result = call_syscall_get_next_entry(fd, info);
                if (result != 1) {
                    break;
                }

                if (info->is_dir) {
                    call_syscall_write("[Directory] ");
                } else {
                    call_syscall_write("[File]      ");
                }
                call_syscall_write(info->name);
                call_syscall_write("\n");
            }
        } else if (memcmp(buffer, "pwd", 3) == 0) {
            call_syscall_write(current_path);
            call_syscall_write("\n");
        } else {
            call_syscall_write("[SHELL] Command '");
            call_syscall_write(buffer);
            call_syscall_write("' not found.\n");
        }
    }
}

void user_process() {
    call_syscall_write("[DEBUG] User process started.\n");

    unsigned long stack_1 = call_syscall_malloc();
    if (stack_1 < 0) {
        call_syscall_write("[ERROR] Cannot allocate stack for process 1.\n");
    }
    call_syscall_clone((unsigned long)&user_process_print, (unsigned long)"1", stack_1);

    unsigned long stack_2 = call_syscall_malloc();
    if (stack_1 < 0) {
        call_syscall_write("[ERROR] Cannot allocate stack for process 1.\n");
    }
    call_syscall_clone((unsigned long)&user_process_print, (unsigned long)"2", stack_2);

    call_syscall_exit();
}

void user_process_fs() {
    int error;
    int fd = call_syscall_open_file("prova.txt", FAT_READ | FAT_WRITE | FAT_CREATE);
    if (fd == -1) {
        call_syscall_write("[ERROR] Cannot open file 'prova.txt'.\n");
        call_syscall_exit();
    }

    int* cnt;

    error = call_syscall_write_file(fd, "ciao! sono il primo processo e ho scritto sul file", 51, cnt);
    if (error) {
        call_syscall_write("[DEBUG] Cannot write on file 'prova.txt'.\n");
    } else {
        call_syscall_write("[DEBUG] File 'prova.txt' written.\n");
    }

    char buffer[51];
    error = call_syscall_read_file(fd, buffer, 51, cnt);
    if (error) {
        call_syscall_write("[ERROR] Cannot read file 'prova.txt'.\n");
    } else {
        call_syscall_write("[DEBUG] File read: the content is '");
        call_syscall_write(buffer);
        call_syscall_write("'\n");
    }

    call_syscall_close_file(fd);
}

void user_process_print(char* process_name) {
    while (1) {
        char buffer[8] = {0};
        call_syscall_input(buffer, 8);
        uart_puts("[P"); uart_puts(process_name); uart_puts("] Hello '"); uart_puts(buffer); uart_puts("'\n");
        call_syscall_yield();
    }
}
