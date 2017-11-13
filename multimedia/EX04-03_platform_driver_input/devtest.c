#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/errno.h>
#include <linux/uaccess.h>
#include <linux/ioport.h>
#include <asm/io.h>
#include <linux/interrupt.h>
#include <plat/irqs.h>
#include <linux/input.h>
#include <linux/platform_device.h>
#include <linux/workqueue.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/slab.h>

#define PAD_PHY_BASE            0x11000000
#define PAD_PHY_SIZE            0x1000
#define GPM4CON                 0x2e0
#define GPM4DAT                 0x2e4

#define DEVICE_NAME "my_pdev"

struct dev_key_info {
	int irq;
	int irq_type;
	char *irq_desc;
	int key_code;
};

struct dev_pdata {
	struct dev_key_info *kinfo;
	int nkeys;
}; 

struct drv_key_info {
	struct input_dev *input;
	struct dev_key_info *kinfo;
	struct work_struct work;
};

struct drv_key_data {
	struct input_dev *input;
	int nkeys;
	struct drv_key_info drv_kinfo[1];
};

static volatile unsigned long pad_base;
static struct resource *pad_mem;

static volatile unsigned int key_value;

static struct workqueue_struct *my_workqueue;
void my_work_function(struct work_struct *work);

void my_work_function(struct work_struct *work)
{
	struct drv_key_info *drv_kinfo = container_of(work, struct drv_key_info, work);
	struct input_dev *input = drv_kinfo->input;
	unsigned int key_code = drv_kinfo->kinfo->key_code;

	printk("devtest: process %d(%s) is running %s()\n", current->pid, current->comm, __FUNCTION__);
	printk("devtest: key_code is %d\n", key_code);

	/* LED ON */
	iowrite32(ioread32(pad_base+GPM4DAT) | (0x3<<4), pad_base+GPM4DAT);

	/* Implement code */

	/* sleep */
	msleep(300);

	/* LED OFF */
	iowrite32(ioread32(pad_base+GPM4DAT) & ~(0x3<<4), pad_base+GPM4DAT);
}

irqreturn_t key_isr(int irq, void *dev_id)
{
	static int count;
	struct drv_key_info *drv_kinfo = (struct drv_key_info *)dev_id;

	printk("devtest: %s(): count = %d: %s\n", __FUNCTION__, ++count, drv_kinfo->kinfo->irq_desc);

        queue_work(my_workqueue, &drv_kinfo->work);

	return IRQ_HANDLED;
}

static void my_release_device(struct device *dev)
{
	printk("devtest: my_release_device\n");
};

struct dev_key_info my_dev_key_info[] = {
	{IRQ_EINT(3), IRQF_TRIGGER_FALLING, "key3_int", KEY_RIGHT},
	{IRQ_EINT(4), IRQF_TRIGGER_FALLING, "key4_int", KEY_LEFT},
};

struct dev_pdata my_dev_pdata = {
	.kinfo = my_dev_key_info,
	.nkeys = ARRAY_SIZE(my_dev_key_info),
};

static struct platform_device my_platform_device = {
	.name           = DEVICE_NAME,
	.id             = 0,
	.num_resources  = 0,
	.dev    = {
		.platform_data = &my_dev_pdata,
		.release = my_release_device,
	},
};

static int driver_probe(struct platform_device *pdev)
{
	struct input_dev *input;
	struct dev_pdata *pdata = pdev->dev.platform_data;
	struct drv_key_data *ddata;
	int i, ret;

	printk("devtest: driver_probe\n");

	pad_mem = request_mem_region(PAD_PHY_BASE, PAD_PHY_SIZE, "pad-base");
	if (pad_mem == NULL) {
		printk("devtest: request_mem_region failed\n");
		ret = -ENOENT;
		goto err_mem_region;
	}

	pad_base = (unsigned long)ioremap(PAD_PHY_BASE, PAD_PHY_SIZE);
	if (pad_base == 0) {
		printk("devtest: ioremap failed\n");
		ret = -EINVAL;
		goto err_ioremap;
	}

        /* LED1 -> GPM4_4 -> OUTPUT LOW -> LED OFF */
        /* LED2 -> GPM4_5 -> OUTPUT LOW -> LED OFF */
	iowrite32((ioread32(pad_base+GPM4CON) & ~(0xff<<16)) | (0x11<<16), pad_base+GPM4CON);
	iowrite32(ioread32(pad_base+GPM4DAT) & ~(0x3<<4), pad_base+GPM4DAT);

	ddata = kzalloc(sizeof(struct drv_key_data) + pdata->nkeys * sizeof(struct drv_key_info), GFP_KERNEL);
	if (ddata == 0) {
		printk("devtest: kzalloc failed\n");
		ret = -ENOMEM;
		goto err_kzalloc;
	}
	platform_set_drvdata(pdev, ddata);

	ddata->nkeys = pdata->nkeys;
	for(i=0; i<ddata->nkeys; i++) {
		ddata->drv_kinfo[i].kinfo = &pdata->kinfo[i];
	}

	my_workqueue = create_singlethread_workqueue("my_wqd");
	if(my_workqueue == NULL) {
		printk("devtest: create_workqueue error\n");
		ret = -ENOMEM;
		goto err_workqueue;
	}

	for(i=0; i<ddata->nkeys; i++) {
		INIT_WORK(&ddata->drv_kinfo[i].work, my_work_function);
	}

	for(i=0; i<ddata->nkeys; i++) {
		struct dev_key_info *kinfo = ddata->drv_kinfo[i].kinfo;
		ret = request_irq(kinfo->irq, key_isr, kinfo->irq_type|IRQF_DISABLED, kinfo->irq_desc, &ddata->drv_kinfo[i]);
		if(ret) {
			printk("devtest: can't enable key interrupt %d!\n", kinfo->irq);
			goto err_irq;
		}
		printk("devtest: key interrupt %d is enabled!\n", kinfo->irq);
	}

	input = input_allocate_device();
	if(!(input)) {
		printk("devtest: input_allocate_device error\n");
		ret = -ENOMEM;
		goto err_input_allocate;
	}
	ddata->input = input;
	for(i=0; i<ddata->nkeys; i++) {
		ddata->drv_kinfo[i].input = input;
	}

	/* Implement code */

	input->name = "my_keypad";
	ret = input_register_device(input);
	if (ret) {
		printk("devtest: Failed to register device\n");
		goto err_input_register;
	}

	printk("devtest: probed successfully\n");

        return 0;

err_input_register:
	input_free_device(input);
err_input_allocate:
	for(i=0; i<ddata->nkeys; i++) {
		struct dev_key_info *kinfo = ddata->drv_kinfo[i].kinfo;
		free_irq(kinfo->irq, kinfo);
	}
err_irq:
	destroy_workqueue(my_workqueue);
err_workqueue:
	iounmap((void *)pad_base);
err_kzalloc:
	kfree(ddata);
err_ioremap:
	release_mem_region(PAD_PHY_BASE, PAD_PHY_SIZE);
err_mem_region:
	return ret;
}

static int driver_remove(struct platform_device *pdev)
{
	struct drv_key_data *ddata = platform_get_drvdata(pdev);
	struct input_dev *input = ddata->input;
	int i;

	printk("devtest: driver_remove\n");

	input_unregister_device(input);
	for(i=0; i<ddata->nkeys; i++) {
		struct dev_key_info *kinfo = ddata->drv_kinfo[i].kinfo;
		free_irq(kinfo->irq, &ddata->drv_kinfo[i]);
	}
	destroy_workqueue(my_workqueue);
	iounmap((void *)pad_base);
	kfree(ddata);
	release_mem_region(PAD_PHY_BASE, PAD_PHY_SIZE);

        return 0;
}

static int driver_suspend(struct platform_device *pdev, pm_message_t pm)
{
        return 0;
}

static int driver_resume(struct platform_device *pdev)
{
        return 0;
}

static struct platform_driver my_platform_driver = {
	.probe          = driver_probe,
	.remove         = driver_remove,
	.suspend        = driver_suspend,
	.resume         = driver_resume,
	.driver         = {
		.name   = DEVICE_NAME,
		.owner  = THIS_MODULE,
	},
};

static int __init platform_device_init(void)
{
	int ret;

	printk("devtest: device_init\n");
	ret = platform_device_register(&my_platform_device);
	if(ret) {
		printk("platform_device_register failed (ret=%d) \n", ret);
		return ret;
	}

	ret = platform_driver_register(&my_platform_driver);
	if(ret) {
		platform_device_unregister(&my_platform_device);
		printk("platform_driver_register failed (ret=%d) \n", ret);
	}

	return ret;
}

static void __exit platform_device_exit(void)
{
	platform_driver_unregister(&my_platform_driver);
	platform_device_unregister(&my_platform_device);

	printk("devtest: device_exit\n");
}

module_init(platform_device_init);
module_exit(platform_device_exit);

MODULE_LICENSE("GPL");

