/*
 * Copyright (c) 2015 Elliptic Technologies Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/io.h>

#include <linux/fs.h>
#include <asm/uaccess.h>
#include <asm/param.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>

#include "elpspacc.h"

struct spacc_priv {
   spacc_device spacc;
   struct semaphore core_running;
   struct tasklet_struct pop_jobs;
   spinlock_t hw_lock;

   unsigned long max_msg_len;
};

/* These are used by spaccdiag, RE and KEP to get the spacc device */
struct platform_device * get_spacc_platdev_by_epn(uint32_t epn, uint32_t virt);
spacc_device * get_spacc_device_by_epn(uint32_t epn, uint32_t virt);


