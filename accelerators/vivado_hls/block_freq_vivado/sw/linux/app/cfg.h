// Copyright (c) 2011-2023 Columbia University, System Level Design Group
// SPDX-License-Identifier: Apache-2.0
#ifndef __ESP_CFG_000_H__
#define __ESP_CFG_000_H__

#include "libesp.h"
#include "block_freq_vivado.h"

typedef int32_t token_t;

/* <<--params-def-->> */
#define DATA_OUT_SIZE 8
#define DATA_IN_SIZE 8192
#define BLOCK_SIZE 32

/* <<--params-->> */
const int32_t data_out_size = DATA_OUT_SIZE;
const int32_t data_in_size = DATA_IN_SIZE;
const int32_t block_size = BLOCK_SIZE;

#define NACC 1

struct block_freq_vivado_access block_freq_cfg_000[] = {
	{
		/* <<--descriptor-->> */
		.data_out_size = DATA_OUT_SIZE,
		.data_in_size = DATA_IN_SIZE,
		.block_size = BLOCK_SIZE,
		.src_offset = 0,
		.dst_offset = 0,
		.esp.coherence = ACC_COH_NONE,
		.esp.p2p_store = 0,
		.esp.p2p_nsrcs = 0,
		.esp.p2p_srcs = {"", "", "", ""},
	}
};

esp_thread_info_t cfg_000[] = {
	{
		.run = true,
		.devname = "block_freq_vivado.0",
		.ioctl_req = BLOCK_FREQ_VIVADO_IOC_ACCESS,
		.esp_desc = &(block_freq_cfg_000[0].esp),
	}
};

#endif /* __ESP_CFG_000_H__ */
