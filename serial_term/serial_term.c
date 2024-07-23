/*
 * stm_lib.c
 *
 *  Created on: 13 июн. 2024 г.
 *      Author: GTR070
 */

#include "gpio.h"
#include "cmsis_os.h"
#include "usart.h"
#include "string.h"
#include "serial_term.h"


#define TERM_QUEUE_TIMEOUT 10
#define TERM_READERS_LEN 10
#define TERM_IN_QUEUE_LEN 10
#define TERM_OUT_QUEUE_LEN TERM_IN_QUEUE_LEN
#define TERM_BUFF_LEN 10
#define TERM_UART_TIMEOUT 1000
#define TERM_CLR "\f"
#define TERM_CRLF "\n\r"

static uint8_t curr_butt_state = 0, prev_butt_state = 0;
static UART_HandleTypeDef *term_uart = NULL;
static uint8_t term_reg_input = 0;
//static char term_buff[TERM_BUFF_LEN];
static uint16_t term_buff_input_pos = 0;

QueueHandle_t q_term_out, q_term_in, q_term_read;
TaskHandle_t t_term_in, t_term_out;
static char term_buff[TERM_BUFF_LEN], term_in;
static char *term_in_ptr = &term_in;

uint8_t readButton(void* gpio, uint16_t pin) {
	curr_butt_state = HAL_GPIO_ReadPin(gpio, pin);

	if(curr_butt_state == 1 && curr_butt_state != prev_butt_state) {
		osDelay(20);
		curr_butt_state = HAL_GPIO_ReadPin(gpio, pin);
	}

	prev_butt_state = curr_butt_state;

	return curr_butt_state;
}


//Serial terminal procedures & functions

inline static void term_buff_clr(void) {
	memset(term_buff, 0, TERM_BUFF_LEN);
}

inline static void term_print_crlf(void) {
	char *crlf = TERM_CRLF;
	xQueueSend(q_term_out, &crlf, TERM_QUEUE_TIMEOUT);
}

void task_term_output(void const * args) {
	char *term_out = NULL;

	for (;;) {
		if (xQueueReceive(q_term_out, &term_out, TERM_QUEUE_TIMEOUT) == pdPASS) {
			HAL_UART_Transmit(term_uart, term_out, strlen(term_out), TERM_UART_TIMEOUT);
		}
	}
}

void task_term_input(void const * args) {
	TaskHandle_t *reading_task;
	for (;;) {
		if ( !(term_reg_input) && (xQueueReceive(q_term_read, &reading_task, TERM_QUEUE_TIMEOUT) == pdPASS) ) {
			term_reg_input = 1;
		}

		HAL_UART_Receive_IT(term_uart, &term_in, 1);

		if (xQueueReceive(q_term_in, &term_in, TERM_QUEUE_TIMEOUT) == pdPASS) {
			if (term_in == '\r') {
				term_print_crlf();
			} else {
				xQueueSend(q_term_out, &term_in_ptr, TERM_QUEUE_TIMEOUT); //echo to terminal
			}

			if (term_reg_input) {
				if (term_in == '\r') {
					vTaskResume(*reading_task);
					term_buff_input_pos = 0;
					term_reg_input = 0;
				} else {
					term_buff[term_buff_input_pos++] = term_in;
				}
			}

		}

	}
}

inline void task_term_input_ISR(void) {
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xQueueSendFromISR(q_term_in, &term_in, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
//	HAL_UART_Receive_IT(term_uart, &term_in, 1);
}


void term_init(UART_HandleTypeDef *huart) {
	if (huart)
		term_uart = huart;

	q_term_in = xQueueCreate(TERM_IN_QUEUE_LEN, sizeof(char));
	q_term_out = xQueueCreate(TERM_OUT_QUEUE_LEN, sizeof(char*));
	q_term_read = xQueueCreate(TERM_READERS_LEN, sizeof(TaskHandle_t*));

	xTaskCreate(task_term_output, "task_term_output", 128, NULL, 0, &t_term_out);
	xTaskCreate(task_term_input, "task_term_input", 128, NULL, 0, &t_term_in);

	term_clear();
}

void term_clear(void) {
	if (term_uart) {
		char *clr = TERM_CLR;
		xQueueSend(q_term_out, &clr, TERM_QUEUE_TIMEOUT);
	}
}

void term_print(const char* out) {
	if (term_uart)
		xQueueSend(q_term_out, &out, TERM_QUEUE_TIMEOUT);
}

void term_println(const char* out) {
	if (term_uart) {
		term_print(out);
		term_print_crlf();
	}
}

void term_readln(char* input) {

	TaskHandle_t t_curr_handle = xTaskGetCurrentTaskHandle();
	TaskHandle_t *t_curr_handle_ptr = &t_curr_handle;

	if (term_uart) {
		term_buff_clr();
		xQueueSend(q_term_read, &t_curr_handle_ptr, TERM_QUEUE_TIMEOUT);
		vTaskSuspend(NULL);
		memcpy(input, term_buff, term_buff_input_pos);
	}
}


