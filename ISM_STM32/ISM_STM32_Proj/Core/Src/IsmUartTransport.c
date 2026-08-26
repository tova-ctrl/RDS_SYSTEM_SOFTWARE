#include "IsmUartTransport.h"
#include "UartProtocol.h"
#include "main.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const int HB_COMMAND_SIZE = 13;
const int STBY_COMMAND_SIZE = 10;
const int ARM_COMMAND_SIZE = 9;
const int SAFE_COMMAND_SIZE = 10;
const int FIRE_COMMAND_SIZE = 10;

// ── FCC Commands consts on ISM side) ───────────
enum {
	HB_COMMAND = 0,
	SAFE_COMMAND = 1,
	STANDBY_COMMAND = 2,
	ARMED_COMMAND = 3,
	FIRING_COMMAND = 4
};

extern UART_HandleTypeDef huart2;
extern void ism_cmd_standby(uint32_t seq);
extern void ism_cmd_arm(uint32_t seq);
extern void ism_cmd_disarm(uint32_t seq);
extern void ism_cmd_fire(uint32_t seq);

static volatile uint8_t s_rx_char;
static volatile char s_rx_line[32];
static volatile int s_rx_index = 0;

void ism_uart_rx_start(void) {
	HAL_UART_Receive_IT(&huart2, (uint8_t*) &s_rx_char, 1);
}
int isItFCCUartCommand(char *command, int len, int ret_val) {
	const char *line = (const char*) s_rx_line;
	char *comm_result = strstr(line, command);
	size_t length = strnlen(s_rx_line, sizeof(s_rx_line));
	return ((length >= len && comm_result != NULL) ? ret_val : -1);
}
int check_isItFCCUartCommand() {
	int is_FCCCommand = -1;
	is_FCCCommand = isItFCCUartCommand("HB:", HB_COMMAND_SIZE, HB_COMMAND);
	if (is_FCCCommand == -1)
		is_FCCCommand = isItFCCUartCommand("STBY:", STBY_COMMAND_SIZE,
				STANDBY_COMMAND);
	if (is_FCCCommand == -1)
		is_FCCCommand = isItFCCUartCommand("ARM:", ARM_COMMAND_SIZE,
				ARMED_COMMAND);
	if (is_FCCCommand == -1)
		is_FCCCommand = isItFCCUartCommand("SAFE:", SAFE_COMMAND_SIZE,
				SAFE_COMMAND);
	if (is_FCCCommand == -1)
		is_FCCCommand = isItFCCUartCommand("FIRE:", FIRE_COMMAND_SIZE,
				FIRING_COMMAND);
	return is_FCCCommand;
}
void ismToFcc_ack_send(const char *line, char *ack_str, char *result,
		char *result_trailer) {
	char chksum_str[3] = { 0 };
	int index1 = result - line;
	int index = result_trailer - result;
	uint8_t expected_crc = uart_crc8((const uint8_t*) line + index1,
			index - 3 + 1);
	strncpy(chksum_str, line + index - 2, 2);
	uint8_t received_crc = (uint8_t) strtoul(chksum_str, NULL, 16);
	//printf("expected_crc: %02X, received_crc: %02X\n", expected_crc, received_crc);
	//expected_crc=received_crc; //==TEMP TOVA 25.8.26
	if (expected_crc == received_crc) {
		char prefix[24];
		int plen = snprintf(prefix, sizeof(prefix), ack_str);
		uint8_t ack_crc = uart_crc8((const uint8_t*) prefix, (uint8_t) plen);
		char ack[32];
		int ack_len = snprintf(ack, sizeof(ack), "%s%02X\n", prefix, ack_crc);
		HAL_UART_Transmit(&huart2, (uint8_t*) ack, (uint16_t) ack_len, 100);
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance != USART2)
		return;

	__HAL_UART_CLEAR_FLAG(huart,
			UART_CLEAR_OREF | UART_CLEAR_NEF | UART_CLEAR_FEF | UART_CLEAR_PEF);

	char c = (char) s_rx_char;
	int is_FCCCommand = check_isItFCCUartCommand();

	if (s_rx_index >= (int) (sizeof(s_rx_line) - 1) || is_FCCCommand != -1
			|| c == '\n' || c == '\r') {
		s_rx_line[s_rx_index + 1] = '\0';
		s_rx_index = 0;

		const char *line = (const char*) s_rx_line;
		//if (((int) is_FCCCommand)!=HB_COMMAND)
			printf("line = %s\r\n", line);
		char *result_trailer = strstr(line, ":AA");
		//char *result2 = strstr(line, "HB:");
		// 2026-08-25: was `result_trailer != NULL || result2 != NULL` — every
		// call to ismToFcc_ack_send() below unconditionally computes
		// `result_trailer - result` (see IsmUartTransport.c's
		// ismToFcc_ack_send) with no NULL check of its own, so a line that
		// matched "HB:" but arrived truncated/corrupted (no ":AA" trailer —
		// e.g. from a noisy/marginal UART connection) let result_trailer
		// through as NULL, producing a huge/wild pointer in
		// `line + index - 2` and a real HardFault (root-caused via a
		// PC/LR/BFAR capture from the new HardFault_Handler — see
		// [[ism-stm32-fault-handlers]] memory). result_trailer is required
		// by every path below, not just some of them, so this must be a hard
		// AND, not an OR: without a checksum trailer there is nothing to
		// validate against anyway, so skipping the whole block is correct,
		// not just crash-avoidance.
		if (result_trailer != NULL){  //|| result2 != NULL) {
			//is_FCCCommand=check_isItFCCUartCommand();
			switch ((int) is_FCCCommand) {
			case HB_COMMAND:
				char *result = strstr(line, "HB:");
				if (result != NULL) {
					ismToFcc_ack_send((const uint8_t*) line, "ACK:211:", result,
							result_trailer);
				}
				break;
			case STANDBY_COMMAND: {
				char *result_stby = strstr(line, "STBY:");
				if (result_stby != NULL) {
					ism_cmd_standby(0);
					ismToFcc_ack_send((const uint8_t*) line, "ACK:103:",
							result_stby, result_trailer);
				}
				break;
			}
			case ARMED_COMMAND: {
				char *result_arm = strstr(line, "ARM:");
				if (result_arm != NULL) {
					ism_cmd_arm(0);
					ismToFcc_ack_send((const uint8_t*) line, "ACK:101:",
							result_arm, result_trailer);
				}
				break;
			}
			case SAFE_COMMAND: {
				char *result_safe = strstr(line, "SAFE:");
				if (result_safe != NULL) {
					ism_cmd_disarm(0);
					ismToFcc_ack_send((const uint8_t*) line, "ACK:102:",
							result_safe, result_trailer);
				}
				break;
			}
			case FIRING_COMMAND: {
				char *result_fire = strstr(line, "FIRE:");
				if (result_fire != NULL) {
					ism_cmd_fire(0);
					ismToFcc_ack_send((const uint8_t*) line, "ACK:120:",
							result_fire, result_trailer);
				}
				break;
			}
			default:
				//if (result2 != NULL) {
				//	ismToFcc_ack_send((const uint8_t*) line, "ACK:211:", result,
				//							result_trailer);
				//}
				break;
			}

		}
		memset((char*) s_rx_line, 0, sizeof(s_rx_line));
	} else if (c != '\r') {
		s_rx_line[s_rx_index++] = c;
	}

	HAL_UART_Receive_IT(&huart2, (uint8_t*) &s_rx_char, 1);
}
