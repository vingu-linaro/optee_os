/*
 * Copyright (c) 2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * OPTEE product in SCP-firmware builds a SCMI server stack for the
 * OP-TEE core firmware. SCMI server comes with fwk and its embedded
 * modules into OP-TEE core.
 */
#include <config.h>
#include <initcall.h>
#include <kernel/panic.h>
#include <arch_main.h>
#include <scmi/scmi_server.h>

/*
 * SCMI server APIs exported to OP-TEE core
 */
int scmi_server_get_channels_count(void)
{
	return optee_get_devices_count();
}

int scmi_server_get_channel(unsigned int id)
{
	int device_id;

	device_id = optee_get_device(id);

	return device_id;
}

void scmi_server_process_thread(unsigned int id, void *memory)
{
	optee_process_message(id, memory);
}

static TEE_Result scmi_server_initialize(void)
{
	int rc = optee_arch_init();

	if (rc < 0) {
		EMSG("SCMI server init failed: %d", rc);
		panic();
	}

	return TEE_SUCCESS;
}
driver_init(scmi_server_initialize);
