#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>
#include <stdint.h>
// Call once after MX_USART2_UART_Init() to arm the first RX interrupt.
void ism_uart_rx_start(void);
int	 isItFCCUartCommand(char* command,int len,const int ret_val);
int	 check_isItFCCUartCommand();
void ismToFcc_ack_send (const char* line,char* ack_str,char* result,char* result_trailer);
#ifdef __cplusplus
}
#endif
