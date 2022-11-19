/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef OPENOCD_JTAG_DRIVERS_CMSIS_DAP_H
#define OPENOCD_JTAG_DRIVERS_CMSIS_DAP_H

#include <stdint.h>

struct cmsis_dap_backend;
struct cmsis_dap_backend_data;

struct pending_transfer_result {
	uint8_t cmd;
	uint32_t data;
	void *buffer;
};

/* Up to MIN(packet_count, MAX_PENDING_REQUESTS) requests may be issued
 * until the first response arrives */
#define MAX_PENDING_REQUESTS 4

struct pending_request_block {
	struct pending_transfer_result *transfers;
	unsigned int transfer_count;
};

struct cmsis_dap {
	struct cmsis_dap_backend_data *bdata;
	const struct cmsis_dap_backend *backend;
	unsigned int packet_size;
	unsigned int packet_usable_size;
	unsigned int packet_buffer_size;
	uint8_t *packet_buffer;
	uint8_t *command;
	uint8_t *response;

	/* Pending requests are organized as a FIFO - circular buffer */
	struct pending_request_block pending_fifo[MAX_PENDING_REQUESTS];
	unsigned int packet_count;
	unsigned int pending_fifo_put_idx, pending_fifo_get_idx;
	unsigned int pending_fifo_block_count;

	uint16_t caps;
	uint8_t mode;
	uint32_t swo_buf_sz;
	bool trace_enabled;
};

struct cmsis_dap_backend {
	const char *name;
	int (*open)(struct cmsis_dap *dap, uint16_t vids[], uint16_t pids[], const char *serial);
	void (*close)(struct cmsis_dap *dap);
	int (*read)(struct cmsis_dap *dap, int timeout_ms);
	int (*write)(struct cmsis_dap *dap, int len, int timeout_ms);
	int (*packet_buffer_alloc)(struct cmsis_dap *dap, unsigned int pkt_sz);
};

extern const struct cmsis_dap_backend cmsis_dap_hid_backend;
extern const struct cmsis_dap_backend cmsis_dap_usb_backend;
extern const struct command_registration cmsis_dap_usb_subcommand_handlers[];

#define REPORT_ID_SIZE   1

#endif
