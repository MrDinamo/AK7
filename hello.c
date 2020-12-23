
/*
 * Copyright (c) 2017, GlobalLogic Ukraine LLC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the GlobalLogic.
 * 4. Neither the name of the GlobalLogic nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY GLOBALLOGIC UKRAINE LLC ``AS IS`` AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL GLOBALLOGIC UKRAINE LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/moduleparam.h>
#include <linux/types.h>

#include "linux/ktime.h"
#include "linux/slab.h"
#include "linux/list.h"

MODULE_AUTHOR("Kovalkov Dima");
MODULE_DESCRIPTION("Hello, world in Linux Kernel Labworks");
MODULE_LICENSE("Dual BSD/GPL");

static uint k = 1;
module_param(k, uint, 0444);
MODULE_PARM_DESC(k, "Number of times that message must be repeated");

struct time_list {
	struct list_head next;
	ktime_t time;
};

static LIST_HEAD(head);

static int __init hello_init(void)
{
	struct time_list *tail;
	uint i;

	if (k == 0) {
		printk(KERN_WARNING "k=%i is 0\n", k);
		return 0;
	} else if (k >= 5 && k <= 10) {
		printk(KERN_WARNING "k=%i in range of 5,10\n", k);
	}

	BUG_ON(k > 10);

	for (i = 0; i < k; i++) {
		tail = kmalloc(sizeof(struct time_list), GFP_KERNEL);

		if (i == 5)
			tail = 0;

		if (ZERO_OR_NULL_PTR(tail))
			goto mem_err;

		tail->time = ktime_get();
		printk(KERN_INFO "Hello, world!\n");

		list_add_tail(&(tail->next), &head);
	}

	return 0;

mem_err:
	{
		struct time_list *md, *tmp;

		printk(KERN_ERR "Runned out of memory");
		list_for_each_entry_safe(md, tmp, &head, next) {
			list_del(&md->next);
			kfree(md);
		}
		BUG();
		return -ENOMEM;
	}
}

static void __exit hello_exit(void)
{
	struct time_list *md, *tmp;

	list_for_each_entry_safe(md, tmp, &head, next) {
			list_del(&md->next);
			kfree(md);
	}
	BUG_ON(!list_empty(&head));
}

module_init(hello_init);
module_exit(hello_exit);
