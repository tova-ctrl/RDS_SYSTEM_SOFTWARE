// Retarget printf / std::cout → USART3 (115200 baud, debug connector on NUCLEO-H753ZI).
#include "stm32h7xx_hal.h"
#include <stdio.h>

extern UART_HandleTypeDef huart3;

// Called by newlib for every character written to stdout/stderr.
int __io_putchar(int ch) {
    uint8_t c = (uint8_t)ch;
    HAL_UART_Transmit(&huart3, &c, 1, 200U);
    return ch;
}

// Called by newlib write() syscall (used by printf, fwrite, std::cout).
int _write(int fd, char* ptr, int len) {
    (void)fd;
    HAL_UART_Transmit(&huart3, (uint8_t*)ptr, (uint16_t)len, 200U);
    return len;
}
