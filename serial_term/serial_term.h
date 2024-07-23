/*
 * serial_term.h
 *
 *  Created on: 13 июн. 2024 г.
 *      Author: GTR070
 */

#ifndef INC_SERIAL_TERM_H_
#define INC_SERIAL_TERM_H_

//char term_input_char;

uint8_t readButton(void* gpio, uint16_t pin);
void term_init(UART_HandleTypeDef *huart);
void term_clear(void);
void term_print(const char* const out);
void term_println(const char* const out);
void term_save_to_buff(void);
void term_readln(char* input);
void task_term_input_ISR(void);


#endif /* INC_SERIAL_TERM_H_ */
