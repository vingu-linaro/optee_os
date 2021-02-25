// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (c) 2019, Linaro Limited
 */
#include <compiler.h>
#include <kernel/pseudo_ta.h>
#include <mm/core_memprot.h>
#include <scmi/pta_scmi_client.h>
#include <scmi/scmi_server.h>
#include <string.h>
#include <tee_api.h>

static TEE_Result cmd_channel_count(void *sess __unused,
				    uint32_t param_types,
				    TEE_Param params[TEE_NUM_PARAMS])
{
	uint32_t expect_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_OUTPUT,
						TEE_PARAM_TYPE_NONE,
						TEE_PARAM_TYPE_NONE,
						TEE_PARAM_TYPE_NONE);

	if (param_types != expect_types)
		return TEE_ERROR_BAD_PARAMETERS;

	params[0].value.a = scmi_server_get_channels_count();

	return TEE_SUCCESS;
}

static TEE_Result cmd_get_channel(void *sess __unused,
				  uint32_t param_types,
				  TEE_Param params[TEE_NUM_PARAMS])
{
	int channel_id = 0;
	uint32_t expect_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INOUT,
						TEE_PARAM_TYPE_NONE,
						TEE_PARAM_TYPE_NONE,
						TEE_PARAM_TYPE_NONE);
	if (param_types != expect_types)
		return TEE_ERROR_BAD_PARAMETERS;

	channel_id = (int)params[0].value.a;

	// Consider only non-secure SCMI shared memory for now
	channel_id = scmi_server_get_channel(channel_id);

	if (channel_id < 0)
		return TEE_ERROR_BAD_PARAMETERS;
	IMSG("SCMI channel: %d", channel_id);

	params[0].value.a = (uint32_t)channel_id;

	return TEE_SUCCESS;
}

static TEE_Result cmd_process_channel(void *sess __unused,
				      uint32_t param_types,
				      TEE_Param params[TEE_NUM_PARAMS])
{
	uint32_t expect_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INPUT,
						TEE_PARAM_TYPE_MEMREF_INOUT,
						TEE_PARAM_TYPE_NONE,
						TEE_PARAM_TYPE_NONE);
	if (param_types == expect_types) {
		DMSG("SCMI process : %"PRIu32"@%p",
		     params[0].value.a, params[1].memref.buffer);

		scmi_server_process_thread(params[0].value.a,
					   params[1].memref.buffer);

		return TEE_SUCCESS;
	}

	return TEE_ERROR_BAD_PARAMETERS;
}

static TEE_Result invoke_command(void *sess, uint32_t cmd,
				 uint32_t param_types,
				 TEE_Param params[TEE_NUM_PARAMS])
{
	DMSG("SCMI command %"PRIx32" ptypes %"PRIx32, cmd, param_types);

	switch (cmd) {
	case PTA_SCMI_CMD_CHANNEL_COUNT:
		return cmd_channel_count(sess, param_types, params);
	case PTA_SCMI_CMD_GET_CHANNEL:
		return cmd_get_channel(sess, param_types, params);
	case PTA_SCMI_CMD_PROCESS_CHANNEL:
		return cmd_process_channel(sess, param_types, params);
	default:
		break;
	}

	return TEE_ERROR_NOT_IMPLEMENTED;
}

pseudo_ta_register(.uuid = PTA_SCMI_UUID, .name = PTA_SCMI_NAME,
		   .flags = PTA_DEFAULT_FLAGS | TA_FLAG_CONCURRENT | TA_FLAG_DEVICE_ENUM,
		   .invoke_command_entry_point = invoke_command);
