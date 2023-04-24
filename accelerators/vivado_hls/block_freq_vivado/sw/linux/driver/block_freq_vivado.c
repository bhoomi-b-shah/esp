// Copyright (c) 2011-2023 Columbia University, System Level Design Group
// SPDX-License-Identifier: Apache-2.0
#include <linux/of_device.h>
#include <linux/mm.h>

#include <asm/io.h>

#include <esp_accelerator.h>
#include <esp.h>

#include "block_freq_vivado.h"

#define DRV_NAME	"block_freq_vivado"

/* <<--regs-->> */
#define BLOCK_FREQ_DATA_OUT_SIZE_REG 0x48
#define BLOCK_FREQ_DATA_IN_SIZE_REG 0x44
#define BLOCK_FREQ_BLOCK_SIZE_REG 0x40

struct block_freq_vivado_device {
	struct esp_device esp;
};

static struct esp_driver block_freq_driver;

static struct of_device_id block_freq_device_ids[] = {
	{
		.name = "SLD_BLOCK_FREQ_VIVADO",
	},
	{
		.name = "eb_76",
	},
	{
		.compatible = "sld,block_freq_vivado",
	},
	{ },
};

static int block_freq_devs;

static inline struct block_freq_vivado_device *to_block_freq(struct esp_device *esp)
{
	return container_of(esp, struct block_freq_vivado_device, esp);
}

static void block_freq_prep_xfer(struct esp_device *esp, void *arg)
{
	struct block_freq_vivado_access *a = arg;

	/* <<--regs-config-->> */
	iowrite32be(a->data_out_size, esp->iomem + BLOCK_FREQ_DATA_OUT_SIZE_REG);
	iowrite32be(a->data_in_size, esp->iomem + BLOCK_FREQ_DATA_IN_SIZE_REG);
	iowrite32be(a->block_size, esp->iomem + BLOCK_FREQ_BLOCK_SIZE_REG);
	iowrite32be(a->src_offset, esp->iomem + SRC_OFFSET_REG);
	iowrite32be(a->dst_offset, esp->iomem + DST_OFFSET_REG);

}

static bool block_freq_xfer_input_ok(struct esp_device *esp, void *arg)
{
	/* struct block_freq_vivado_device *block_freq = to_block_freq(esp); */
	/* struct block_freq_vivado_access *a = arg; */

	return true;
}

static int block_freq_probe(struct platform_device *pdev)
{
	struct block_freq_vivado_device *block_freq;
	struct esp_device *esp;
	int rc;

	block_freq = kzalloc(sizeof(*block_freq), GFP_KERNEL);
	if (block_freq == NULL)
		return -ENOMEM;
	esp = &block_freq->esp;
	esp->module = THIS_MODULE;
	esp->number = block_freq_devs;
	esp->driver = &block_freq_driver;
	rc = esp_device_register(esp, pdev);
	if (rc)
		goto err;

	block_freq_devs++;
	return 0;
 err:
	kfree(block_freq);
	return rc;
}

static int __exit block_freq_remove(struct platform_device *pdev)
{
	struct esp_device *esp = platform_get_drvdata(pdev);
	struct block_freq_vivado_device *block_freq = to_block_freq(esp);

	esp_device_unregister(esp);
	kfree(block_freq);
	return 0;
}

static struct esp_driver block_freq_driver = {
	.plat = {
		.probe		= block_freq_probe,
		.remove		= block_freq_remove,
		.driver		= {
			.name = DRV_NAME,
			.owner = THIS_MODULE,
			.of_match_table = block_freq_device_ids,
		},
	},
	.xfer_input_ok	= block_freq_xfer_input_ok,
	.prep_xfer	= block_freq_prep_xfer,
	.ioctl_cm	= BLOCK_FREQ_VIVADO_IOC_ACCESS,
	.arg_size	= sizeof(struct block_freq_vivado_access),
};

static int __init block_freq_init(void)
{
	return esp_driver_register(&block_freq_driver);
}

static void __exit block_freq_exit(void)
{
	esp_driver_unregister(&block_freq_driver);
}

module_init(block_freq_init)
module_exit(block_freq_exit)

MODULE_DEVICE_TABLE(of, block_freq_device_ids);

MODULE_AUTHOR("Emilio G. Cota <cota@braap.org>");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("block_freq_vivado driver");
