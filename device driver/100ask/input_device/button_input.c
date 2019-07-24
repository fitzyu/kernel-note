/*
 * 参考gpio_keys.c
 */
#include <linux/module.h>

struct pin_desc {
	char *name;
	unsigned int pin;
	unsigned int keyValue;
};

static struct input_dev *buttons_dev;
static struct pin_desc *irq_pd;
static struct timer_list button_timer;

struct pin_desc pins_desc[4] = {
	{"S2", EXYNOS4_GPX3(2), KEY_L},
	{"S3", EXYNOS4_GPX3(3), KEY_S},
	{"S4", EXYNOS4_GPX3(4), KEY_ENTER},
	{"S5", EXYNOS4_GPX3(5), KEY_LEFTSHIFT},
};

static irqreturn_t button_irq(int irq, void *dev_id)
{
	/* 10ms 后启动定时器 */
	irq_pd = (struct desc *)dev_id;
	mod_timer(&button_timer, jiffies + HZ / 100);
	return IRQ_RETVAL(IRQ_HANDLED);
}

static void button_timer_function(unsigned long data)
{
	struct pin_desc *pinDesc = irq_pd;
	printk("irq = %d\n", irq);

	if (!irq_pd) {
		return;
	}
	int pinValue = gpio_get_value(pinDesc->pin);
	if (pinValue) { // 鏉惧紑
		input_event(buttons_dev, EV_KEY, pinDesc->key_val, 0); // 0表示松开 1表示按下
		input_sync(buttons_dev);
	} else { // 鎸変笅
		input_event(buttons_dev, EV_KEY, pinDesc->key_val, 1);
		input_sync(buttons_dev);
	}

}

static int __init buttons_init(void)
{
	int i;

	/* 1. 分配一个input_dev结构体 */
	buttons_dev = input_allocate_device();
	/*2. 设置*/
	/* 2.1 能产生按键类事件 */
	set_bit(EV_KEY, buttons_dev->evbit);

	/* 2.2 能产生这类操作里的哪些事件: L S ENTER LEFTSHIFT */
	set_bit(KEY_L, buttons_dev->evbit);
	set_bit(KEY_S, buttons_dev->evbit);
	set_bit(KEY_ENTER, buttons_dev->evbit);
	set_bit(KEY_LEFTSHIFT, buttons_dev->evbit);

	/* 3. 注册 */
	input_register_device(&buttons_dev);

	/* 4. 硬件相关的操作 */
	init_timer(&button_timer);
	button_timer.function = (void (*) (unsigned long)) button_timer_function;
	add_timer(&button_timer);
	
	for (i = 0; i < 4; i++) {
		irq = gpio_to_irq(pins_desc[i].pin);
		ret = request_irq(irq, button_irq, IRQF_SHARED, pins_desc[i].name, (void *)&pins_desc[i]);
		if (ret < 0) {
			return ret;
		}
	}
	return 0;
]

static void __exit buttons_exit(void)
{
	int i;

	/* 1. 释放中断 */
	for (i = 0; i < 4; i++) {
		irq = gpio_to_irq(pins_desc[i].pin);
		free_irq(irq, (void *)&pins_desc[i]);
	}

	del_timer(&button_timer);

	input_unregister_device(&buttons_dev);

	input_free_device(&buttons_dev);
}

module_init(buttons_init);
module_exit(buttons_exit);
MODULE_LICENSE("GPL");
