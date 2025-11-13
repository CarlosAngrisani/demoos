#ifndef __MMIO_H
 #define __MMIO_H

 #include <stdint.h>

 #define MMIO_BASE 0x3F000000

 static inline void mmio_write(uint32_t reg, uint32_t data)
 {
     *(volatile uint32_t*)(MMIO_BASE + reg) = data;
 }

 static inline uint32_t mmio_read(uint32_t reg)
 {
     return *(volatile uint32_t*)(MMIO_BASE + reg);
 }

 enum {
     GPIO_BASE   = 0x200000,
     GPPUD       = (GPIO_BASE + 0x94),
     GPPUDCLK0   = (GPIO_BASE + 0x98),
 };

 #endif

