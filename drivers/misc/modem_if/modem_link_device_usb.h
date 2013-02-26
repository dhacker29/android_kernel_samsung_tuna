/*
 * Copyright (C) 2010 Google, Inc.
 * Copyright (C) 2010 Samsung Electronics.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef __MODEM_LINK_DEVICE_USB_H__
#define __MODEM_LINK_DEVICE_USB_H__

#include <linux/usb.h>
#ifdef CONFIG_HAS_WAKELOCK
#include <linux/wakelock.h>

#define DEFAULT_RAW_WAKE_TIME (6*HZ)
#define DEFAULT_FMT_WAKE_TIME (HZ/2)
#define SVNET_SUSPEND_UNLOCK_DELAY msecs_to_jiffies(20)
#endif

#define IF_USB_NOT_MAIN		1
#define IF_USB_DEVNUM_MAX	3
#define IF_USB_DEV_ADDR		0xa0

#define IF_USB_BOOT_EP		0
#define IF_USB_FMT_EP		0
#define IF_USB_RAW_EP		1
#define IF_USB_RFS_EP		2

#define AUTOSUSPEND_DELAY_MS		500
#define HOST_WAKEUP_TIMEOUT_MS		2000

#define RX_BUFSIZE_RFS			(256 * 1024)
#define RX_BUFSIZE_RAW			(16 * 1024)

enum RESUME_STATUS {
	CP_INITIATED_RESUME,
	AP_INITIATED_RESUME,
};

struct if_usb_devdata {
	struct usb_interface *data_intf;
	unsigned int tx_pipe;
	unsigned int rx_pipe;
	u8 disconnected;

	int format;
	struct urb *urb;
	void *rx_buf;
	unsigned int rx_buf_size;
};

struct usb_link_device {
	/*COMMON LINK DEVICE*/
	struct link_device ld;

	struct modem_data *pdata;

	/*USB SPECIFIC LINK DEVICE*/
	struct usb_device	*usbdev;
	struct if_usb_devdata	devdata[IF_USB_DEVNUM_MAX];
	struct work_struct	post_resume_work;
	struct delayed_work	reconnect_work;
	struct delayed_work	runtime_pm_work;

	struct wake_lock	gpiolock;
	struct wake_lock	susplock;

	unsigned long		driver_info;
	unsigned int		dev_count;
	unsigned int		suspended;
	atomic_t		suspend_count;
	enum RESUME_STATUS	resume_status;
	int if_usb_connected;
	int reconnect_cnt;
	int flow_suspend;
	struct urb		*urbs[0];

	const struct attribute_group *group;
	unsigned gpio_cp_off;
	unsigned gpio_slave_wakeup;
	unsigned gpio_host_wakeup;
	unsigned gpio_host_active;
	int irq_host_wakeup;
	struct delayed_work dwork;
	struct work_struct resume_work;
	int wakeup_flag; /*flag for CP boot GPIO sync flag*/
	int cpcrash_flag;
	wait_queue_head_t l2_wait;
	int irq[3];

	spinlock_t		lock;
	struct usb_anchor	deferred;

	/*COMMON LINK DEVICE*/
	/* maybe -list of io devices for the link device to use */
	/* to find where to send incoming packets to */
	struct list_head list_of_io_devices;
};
/* converts from struct link_device* to struct xxx_link_device* */
#define to_usb_link_device(linkdev) \
			container_of(linkdev, struct usb_link_device, ld)
#endif

#define SET_SLAVE_WAKEUP(_pdata, _value)			\
do {								\
	gpio_set_value(_pdata->gpio_slave_wakeup, _value);	\
	pr_debug("> S-WUP %s\n", _value ? "1" : "0");	\
} while (0)

#define SET_HOST_ACTIVE(_pdata, _value)			\
do {								\
	gpio_set_value(_pdata->gpio_host_active, _value);	\
	pr_debug("> H-ACT %s\n", _value ? "1" : "0");	\
} while (0)

