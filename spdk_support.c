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

enum spdk_iostat_bdev_major_number {
    SPDK_IOSTAT_BDEV_NVME = 259,
    SPDK_IOSTAT_BDEV_MALLOC = 260,
    SPDK_IOSTAT_BDEV_AIO = 261,
};

static struct spdk_jsonrpc_client *client = NULL;
static const char *g_rpcsock_addr = SPDK_DEFAULT_RPC_ADDR;
static int g_addr_family = AF_UNIX;

int
spdk_iostat_parse_bdev_name(char *bdev_name, int *major, int *minor)
{
    if (bdev_name == NULL) {
        return 0;
    }

    if (strncmp("Nvme", bdev_name, 4) == 0) {
        *major = SPDK_IOSTAT_BDEV_NVME;
        *minor = bdev_name[4] - '0';
    } else if (strncmp("Malloc", bdev_name, 6) == 0) {
        *major = SPDK_IOSTAT_BDEV_MALLOC;
        *minor = bdev_name[6] - '0';
    } else if (strncmp("AIO", bdev_name, 3) == 0) {
        *major = SPDK_IOSTAT_BDEV_AIO;
        *minor = bdev_name[3] - '0';
    } else {
        return 0;
    }
    return 1;
}

int
spdk_iostat_client_connect(void)
{
    client = spdk_jsonrpc_client_connect(g_rpcsock_addr, g_addr_family);
    if (!client) {
        fprintf(stderr, "spdk_jsonrpc_client_connect() faild: %d\n", errno);
        return 0;
    }
    return 1;
}

void
spdk_iostat_client_close(void)
{
    if (client) {
        spdk_jsonrpc_client_close(client);
        client = NULL;
    }
}
