#ifndef _SPDK_SUPPORT_H
#define _SPDK_SUPPORT_H

#include <stdbool.h>
#include <stdint.h>

#define MAX_BDEV_NUM 32
#define MAX_BDEV_NAME_LEN 128

struct spdk_iostat_info {
	char bdev_name[MAX_BDEV_NAME_LEN];

	unsigned long rd_sectors;
	unsigned long wr_sectors;
	unsigned long dc_sectors;

	unsigned long rd_ios;
	unsigned long wr_ios;
	unsigned long dc_ios;

	unsigned long rd_merges;
	unsigned long wr_merges;
	unsigned long dc_merges;

	unsigned int rd_ticks;
	unsigned int wr_ticks;
	unsigned int dc_ticks;

	unsigned int ios_pgr;
	unsigned int tot_ticks;
	unsigned int rq_ticks;
};

int spdk_iostat_get_bdevs_iostat(struct spdk_iostat_info *io_info);

int spdk_iostat_get_bdevs_number(void);

int spdk_iostat_parse_bdev_name(char *bdev_name, int *major_number, int *minor_number);

int spdk_iostat_client_connect(void);

void spdk_iostat_client_close(void);

#endif
