#include "sd_filesystem.h"
#include "sd.h"
#include "../../libs/fat32/fat.h"
#include "../../drivers/uart/uart.h"

int sd_read_adapter(uint8_t* buffer, uint32_t sector) {
    sd_readblock(sector, buffer, 1);
}

int sd_write_adapter(const uint8_t* buffer, uint32_t sector) {
    sd_writeblock(buffer, sector, 1);
}

int sd_filesystem_init() {
    DiskOps d = {sd_read_adapter, sd_write_adapter};
    Fat fat;
    fat_mount(&d, 0, &fat, "");

    d.write("Hello world from the SD!\n", 1);
    char* buffer;
    d.read(buffer, 1);
    
    uart_puts(buffer);

    return SD_FILESYSTEM_INIT_OK;
}
