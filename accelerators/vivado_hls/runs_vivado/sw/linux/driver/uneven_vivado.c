// Copyright (c) 2011-2023 Columbia University, System Level Design Group
// SPDX-License-Identifier: Apache-2.0
#include <linux/of_device.h>
#include <linux/mm.h>

#include <asm/io.h>

#include <esp_accelerator.h>
#include <esp.h>

#include "uneven_vivado.h"

#define DRV_NAME	"uneven_vivado"

/* <<--regs-->> */
#define UNEVEN_DATA_OUT_SIZE_REG 0x44
#define UNEVEN_DATA_IN_SIZE_REG 0x40

struct uneven_vivado_device {
	struct esp_device esp;
};

static struct esp_driver uneven_driver;

static struct of_device_id uneven_device_ids[] = {
	{
		.name = "SLD_UNEVEN_VIVADO",
	},
	{
		.name = "eb_75",
	},
	{
		.compatible = "sld,uneven_vivado",
	},
	{ },
};

static int uneven_devs;

static inline struct uneven_vivado_device *to_uneven(struct esp_device *esp)
{
	return container_of(esp, struct uneven_vivado_device, esp);
}

static void uneven_prep_xfer(struct esp_device *esp, void *arg)
{
	struct uneven_vivado_access *a = arg;

	/* <<--regs-config-->> */
	iowrite32be(a->data_out_size, esp->iomem + UNEVEN_DATA_OUT_SIZE_REG);
	iowrite32be(a->data_in_size, esp->iomem + UNEVEN_DATA_IN_SIZE_REG);
	iowrite32be(a->src_offset, esp->iomem + SRC_OFFSET_REG);
	iowrite32be(a->dst_offset, esp->iomem + DST_OFFSET_REG);

}

static bool uneven_xfer_input_ok(struct esp_device *esp, void *arg)
{
	/* struct uneven_vivado_device *uneven = to_uneven(esp); */
	/* struct uneven_vivado_access *a = arg; */

	return true;
}

static int uneven_probe(struct platform_device *pdev)
{
	struct uneven_vivado_device *uneven;
	struct esp_device *esp;
	int rc;

	uneven = kzalloc(sizeof(*uneven), GFP_KERNEL);
	if (uneven == NULL)
		return -ENOMEM;
	esp = &uneven->esp;
	esp->module = THIS_MODULE;
	esp->number = uneven_devs;
	esp->driver = &uneven_driver;
	rc = esp_device_register(esp, pdev);
	if (rc)
		goto err;

	uneven_devs++;
	return 0;
 err:
	kfree(uneven);
	return rc;
}

static int __exit uneven_remove(struct platform_device *pdev)
{
	struct esp_device *esp = platform_get_drvdata(pdev);
	struct uneven_vivado_device *uneven = to_uneven(esp);

	esp_device_unregister(esp);
	kfree(uneven);
	return 0;
}

static struct esp_driver uneven_driver = {
	.plat = {
		.probe		= uneven_probe,
		.remove		= uneven_remove,
		.driver		= {
			.name = DRV_NAME,
			.owner = THIS_MODULE,
			.of_match_table = uneven_device_ids,
		},
	},
	.xfer_input_ok	= uneven_xfer_input_ok,
	.prep_xfer	= uneven_prep_xfer,
	.ioctl_cm	= UNEVEN_VIVADO_IOC_ACCESS,
	.arg_size	= sizeof(struct uneven_vivado_access),
};

static int __init uneven_init(void)
{
	return esp_driver_register(&uneven_driver);
}

static void __exit uneven_exit(void)
{
	esp_driver_unregister(&uneven_driver);
}

module_init(uneven_init)
module_exit(uneven_exit)

MODULE_DEVICE_TABLE(of, uneven_device_ids);

MODULE_AUTHOR("Emilio G. Cota <cota@braap.org>");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("uneven_vivado driver");
