// Copyright (c) 2011-2023 Columbia University, System Level Design Group
// SPDX-License-Identifier: Apache-2.0
#ifndef _BLOCK_FREQ_VIVADO_H_
#define _BLOCK_FREQ_VIVADO_H_

#ifdef __KERNEL__
#include <linux/ioctl.h>
#include <linux/types.h>
#else
#include <sys/ioctl.h>
#include <stdint.h>
#ifndef __user
#define __user
#endif
#endif /* __KERNEL__ */

#include <esp.h>
#include <esp_accelerator.h>

struct block_freq_vivado_access {
	struct esp_access esp;
	/* <<--regs-->> */
	unsigned data_out_size;
	unsigned data_in_size;
	unsigned block_size;
	unsigned src_offset;
	unsigned dst_offset;
};

#define BLOCK_FREQ_VIVADO_IOC_ACCESS	_IOW ('S', 0, struct block_freq_vivado_access)

#endif /* _BLOCK_FREQ_VIVADO_H_ */
