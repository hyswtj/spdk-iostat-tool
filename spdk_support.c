#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "spdk/stdinc.h"
#include "spdk/event.h"
#include "spdk/jsonrpc.h"
#include "spdk/util.h"
#include "spdk/rpc.h"

#include "cJSON.h"
#include "spdk_support.h"

#define SPDK_RPC_GET_BDEVS_IOSTAT "get_bdevs_iostat"
#define MAX_RESPONSE_SIZE 8192

enum spdk_iostat_bdev_major_number {
	SPDK_IOSTAT_BDEV_NVME = 259,
	SPDK_IOSTAT_BDEV_MALLOC = 260,
	SPDK_IOSTAT_BDEV_AIO = 261,
};

static struct spdk_jsonrpc_client *g_client = NULL;
static const char *g_rpcsock_addr = SPDK_DEFAULT_RPC_ADDR;
static int g_addr_family = AF_UNIX;

static int
spdk_rpc_get_bdevs_iostat(char *resp)
{
	int rc;
	struct spdk_jsonrpc_client_response *json_resp = NULL;
	struct spdk_jsonrpc_client_request *request;
	struct spdk_json_write_ctx *w;

	request = spdk_jsonrpc_client_create_request();
		if (request == NULL) {
		return -ENOMEM;
	}

	assert(g_client != NULL);

	w = spdk_jsonrpc_begin_request(request, 1, SPDK_RPC_GET_BDEVS_IOSTAT);
	spdk_jsonrpc_end_request(request, w);
	spdk_jsonrpc_client_send_request(g_client, request);

	do {
		rc = spdk_jsonrpc_client_poll(g_client, 1);
	} while (rc == 0 || rc == -ENOTCONN);

	if (rc <=0) {
		rc = -1;
		goto out;
	}

	json_resp = spdk_jsonrpc_client_get_response(g_client);
	if (json_resp == NULL) {
		rc = -1;
		goto out;
	}

	if (json_resp->error != NULL) {
		rc = -1;
		goto out;
	}

	assert(json_resp->result);
	strcpy(resp, json_resp->result->start);
	rc = 0;

out:
	spdk_jsonrpc_client_free_response(json_resp);
	return rc;
}

int
spdk_iostat_get_bdevs_iostat(struct spdk_iostat_info *io_info)
{
	int i, rc, index = 0, size = 0;
	char *resp;
	cJSON *json, *item, *obj_item;

	if (!g_client) {
		return -1;
	}

	resp = calloc(1, MAX_RESPONSE_SIZE);
	if (!resp) {
		return -ENOMEM;
	}
	rc = spdk_rpc_get_bdevs_iostat(resp);
	if (rc != 0) {
		return -1;
	}

	json = cJSON_Parse(resp);
	size = cJSON_GetArraySize(json);

	/* Skip first item since that's "tick_rate" insead of bdev */
	for (i = 1; i < size; i++) {
		item = cJSON_GetArrayItem(json, i);

		obj_item = cJSON_GetObjectItem(item, "name");
		strcpy(io_info[index].bdev_name, obj_item->valuestring);

		obj_item = cJSON_GetObjectItem(item, "bytes_read");
		io_info[index].rd_sectors = obj_item->valueint >> 9;

		obj_item = cJSON_GetObjectItem(item, "num_read_ops");
		io_info[index].rd_ios = obj_item->valueint;

		obj_item = cJSON_GetObjectItem(item, "bytes_written");
		io_info[index].wr_sectors = obj_item->valueint >> 9;

		obj_item = cJSON_GetObjectItem(item, "num_write_ops");
		io_info[index].wr_ios = obj_item->valueint;

		// TODO: more infomation needed
		io_info[index].dc_sectors = 0;
		io_info[index].dc_ios = 0;

		io_info[index].rd_merges = 0;
		io_info[index].wr_merges = 0;
		io_info[index].dc_merges = 0;

		io_info[index].rd_ticks = 0;
		io_info[index].wr_ticks = 0;
		io_info[index].dc_ticks = 0;

		io_info[index].ios_pgr = 0;
		io_info[index].tot_ticks = 0;
		io_info[index].rq_ticks = 0;

		index++;
	}

	if (size > 0) {
		size--;
	}
	free(resp);
	cJSON_Delete(json);
	return size;
}

int
spdk_iostat_get_bdevs_number(void)
{
	int number = 0, rc;
	cJSON *json;
	char *resp;

	if (!g_client) {
		return 0;
	}

	resp = calloc(1, MAX_RESPONSE_SIZE);
	if (!resp) {
		return -ENOMEM;
	}

	rc = spdk_rpc_get_bdevs_iostat(resp);
	if (rc == 0) {
		json = cJSON_Parse(resp);
		number = cJSON_GetArraySize(json);
	}

	/* Decrease it since JSON response contains extra object "tick_rate". */
	if (number >= 1) {
		number--;
	}

	free(resp);
	return number;
}

int
spdk_iostat_parse_bdev_name(char *bdev_name, int *major_number, int *minor_number)
{
	if (bdev_name == NULL) {
		return -1;
	}

	if (strncmp("Nvme", bdev_name, 4) == 0) {
		*major_number = SPDK_IOSTAT_BDEV_NVME;
		*minor_number = bdev_name[4] - '0';
	} else if (strncmp("Malloc", bdev_name, 6) == 0) {
		*major_number = SPDK_IOSTAT_BDEV_MALLOC;
		*minor_number = bdev_name[6] - '0';
	} else if (strncmp("AIO", bdev_name, 3) == 0) {
		*major_number = SPDK_IOSTAT_BDEV_AIO;
		*minor_number = bdev_name[3] - '0';
	} else {
		return -1;
	}
	return 0;
}

int
spdk_iostat_client_connect(void)
{
	g_client = spdk_jsonrpc_client_connect(g_rpcsock_addr, g_addr_family);
	if (!g_client) {
		fprintf(stderr, "spdk_jsonrpc_client_connect() faild: %d\n", errno);
		return -1;
	}
	return 0;
}

void
spdk_iostat_client_close(void)
{
	if (g_client) {
		spdk_jsonrpc_client_close(g_client);
		g_client = NULL;
	}
}
