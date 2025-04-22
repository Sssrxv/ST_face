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
 *
 * This structure is used to control the /dev/spaccirq interface to reprogram
 * the IRQ handler
 */

#ifndef SPACC_IRQ_H_
#define SPACC_IRQ_H_

typedef struct {
   uint32_t spacc_epn, spacc_virt,  // identify which spacc
            command,                // what operation
            irq_mode,
            wd_value,
            stat_value,
            cmd_value;
} elpspacc_irq_ioctl;

enum {
   SPACC_IRQ_MODE_WD=1, // use WD
   SPACC_IRQ_MODE_STEP, // older use CMD/STAT stepping
};

enum {
   SPACC_IRQ_CMD_GET=0,
   SPACC_IRQ_CMD_SET=1
};

#endif
