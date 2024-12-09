// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/moduleparam.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/ktime.h>

MODULE_AUTHOR("Serhii Popovych <serhii.popovych@globallogic.com>");
MODULE_DESCRIPTION("Hello, world with parameters and event list");
MODULE_LICENSE("Dual BSD/GPL");

static uint print_count = 1;
module_param(print_count, uint, 0444);
MODULE_PARM_DESC(print_count, "Number of times to print 'Hello, world!'");

struct hello_event {
	struct list_head list;
	ktime_t time;
};

static LIST_HEAD(hello_list);

static int __init hello_init(void)
{
	uint i;
	struct hello_event *event;

	if (print_count == 0 || (print_count >= 5 && print_count <= 10)) {
		pr_warn("Warning: Parameter value is %u, but continuing.\n",
			print_count);
	} else if (print_count > 10) {
		pr_err("Error: Parameter value %u is too large. Module not loaded.\n",
		       print_count);
		return -EINVAL;
	}

	for (i = 0; i < print_count; i++) {
		event = kmalloc(sizeof(*event), GFP_KERNEL);
		if (!event)
			return -ENOMEM;

		event->time = ktime_get();
		list_add_tail(&event->list, &hello_list);

		pr_info("Hello, world! #%u\n", i + 1);
	}

	return 0;
}

static void __exit hello_exit(void)
{
	struct hello_event *event, *tmp;

	list_for_each_entry_safe(event, tmp, &hello_list, list) {
		pr_info("Event time: %llu ns\n", ktime_to_ns(event->time));
		list_del(&event->list);
		kfree(event);
	}
	pr_info("Goodbye, world!\n");
}

module_init(hello_init);
module_exit(hello_exit);

