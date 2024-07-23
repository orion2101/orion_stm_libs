/*
 * can_utils.h
 *
 *  Created on: Jul 22, 2024
 *      Author: UTR070
 */

#ifndef UTILS_CAN_UTILS_CAN_UTILS_H_
#define UTILS_CAN_UTILS_CAN_UTILS_H_

//0x1FFFFFFF

typedef enum {
	CAN_FILTER_LIST_16 	= 0x00, ///< 16 bit filter register with list filtering mode and standard IDs
	CAN_FILTER_MASK_16 	= 0x01, ///< 16 bit filter register with mask filtering mode and standard IDs
	CAN_FILTER_LIST_32 	= 0x02, ///< 32 bit filter register with list filtering mode and standard IDs
	CAN_FILTER_MASK_32 	= 0x03, ///< 32 bit filter register with mask filtering mode and standard IDs
	CAN_FILTER_LIST_32x = 0x04, ///< 32 bit filter register with list filtering mode and extended IDs
	CAN_FILTER_MASK_32x = 0x05, ///< 32 bit filter register with mask filtering mode and extended IDs
} CAN_FilterMode_t;


void filter_config(CAN_FilterTypeDef *filter, CAN_FilterMode_t mode, uint8_t fifo_ix, uint8_t bank_ix, uint8_t bank_cnt, uint32_t id, uint32_t mask);
void header_config(CAN_TxHeaderTypeDef *header, uint8_t id_type, uint8_t data_type, uint32_t id, uint8_t data_len, uint8_t TGT);


#endif /* UTILS_CAN_UTILS_CAN_UTILS_H_ */
