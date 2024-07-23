/*
 * can_utils.c
 *
 *  Created on: Jul 22, 2024
 *      Author: UTR070
 */


#include "can.h"
#include "can_utils.h"

/**
 * @brief	Set CAN	filter.
 * @details
 *
 * @param	filter		- A pointer to CAN_FilterTypeDef.
 * @param	mode		- Filter mode settings. @ref CAN_FilterMode_t.
 * @param	fifo_ix		- Reception FIFO index (0, 1).
 * @param	bank_ik		- Start filter bank.
 * @param	bank_cnt 	- End filter bank.
 * @param	id			- One 32 bit or two 16 bit CAN ID(s) or mask, depending on _mode_. @ref CAN_FilterMode_t.
 * @param	mask		- One 32 bit or two 16 bit CAN ID(s) or mask, depending on _mode_. @ref CAN_FilterMode_t.
 */
void filter_config(CAN_FilterTypeDef *filter, CAN_FilterMode_t mode, uint8_t fifo_ix, uint8_t bank_ix, uint8_t bank_cnt, uint32_t id, uint32_t mask) {

	if (mode == CAN_FILTER_MASK_32x || mode == CAN_FILTER_LIST_32x) {
		filter->FilterMode = (mode == CAN_FILTER_MASK_32x) ? CAN_FILTERMODE_IDMASK : CAN_FILTERMODE_IDLIST;
		filter->FilterScale = CAN_FILTERSCALE_32BIT;
		filter->FilterIdHigh = (uint16_t)(id >> 13);
		filter->FilterIdLow = (uint16_t)(id << 3) | 0x04;
		filter->FilterMaskIdHigh = (uint16_t)(mask >> 13);
		filter->FilterMaskIdLow = (uint16_t)(mask << 3) | 0x04;

	} else if (mode == CAN_FILTER_MASK_32 || mode == CAN_FILTER_LIST_32) {
		filter->FilterMode = (mode == CAN_FILTER_MASK_32) ? CAN_FILTERMODE_IDMASK : CAN_FILTERMODE_IDLIST;
		filter->FilterScale = CAN_FILTERSCALE_32BIT;
		filter->FilterIdHigh = (uint16_t)(id << 5);
		filter->FilterIdLow = 0;
		filter->FilterMaskIdHigh = (uint16_t)(mask << 5);
		filter->FilterMaskIdLow = 0;

	} else if (mode == CAN_FILTER_MASK_16 || mode == CAN_FILTER_LIST_16) {
		filter->FilterMode = (mode == CAN_FILTER_MASK_16) ? CAN_FILTERMODE_IDMASK : CAN_FILTERMODE_IDLIST;
		filter->FilterScale = CAN_FILTERSCALE_16BIT;
		filter->FilterIdHigh = (uint16_t)(id << 5);
		filter->FilterIdLow = (uint16_t)((id & 0x07FF0000) >> 11);
		filter->FilterMaskIdHigh = (uint16_t)(mask << 5);
		filter->FilterMaskIdLow = (uint16_t)((mask & 0x07FF0000) >> 11);
	}

	filter->FilterBank = (uint32_t)bank_ix;
	filter->SlaveStartFilterBank = (uint32_t)bank_cnt;
	filter->FilterFIFOAssignment = (uint32_t)fifo_ix;
	filter->FilterActivation = ENABLE;
}

/**
 * @brief
 * @details
 *
 * @param	header		- A pointer to CAN_TxHeaderTypeDef.
 * @param	id_type		- Standard ID = 0; Extended ID = 1.
 * @param	data_type	- Data frame = 0; Remote frame = 1.
 * @param	id			- Frame ID.
 * @param	data_len	- Frame length. The maximum value being 8.
 * @param	TGT			- TransmissionGlobalTime for Time Triggered Communication.
 */
void header_config(CAN_TxHeaderTypeDef *header, uint8_t id_type, uint8_t data_type, uint32_t id, uint8_t data_len, uint8_t TGT) {
	if (id_type == 0) {
		header->IDE = CAN_ID_STD;
		header->StdId = id;
	} else {
		header->IDE = CAN_ID_EXT;
		header->ExtId = id;
	}

	header->TransmitGlobalTime = (TGT == 0) ? DISABLE : ENABLE;
	header->RTR = (data_type == 0) ? CAN_RTR_DATA : CAN_RTR_REMOTE;
	header->DLC = data_len;

}
