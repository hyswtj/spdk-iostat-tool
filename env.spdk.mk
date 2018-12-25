SPDK_ROOT_DIR ?= $(abspath $(CURDIR)/../spdk)
SPDK_LIB_DIR ?= $(SPDK_ROOT_DIR)/build/lib

include $(SPDK_ROOT_DIR)/mk/config.mk

DPDK_LIB_DIR ?= $(CONFIG_DPDK_DIR)/lib
DPDK_LIB_LIST = -lrte_eal -lrte_mempool -lrte_ring -lrte_pci -lrte_bus_pci

ifneq (, $(wildcard $(DPDK_LIB_DIR)/librte_kvargs.*))
DPDK_LIB_LIST += -lrte_kvargs
endif

override CFLAGS += -I$(SPDK_ROOT_DIR)/include
override LDFLAGS += \
	-Wl,--whole-archive \
	-L$(SPDK_LIB_DIR) -lspdk_log -lspdk_sock -lspdk_nvme \
	-lspdk_env_dpdk -lspdk_util -lspdk_jsonrpc -lspdk_json \
	-lspdk_rpc \
	-L$(DPDK_LIB_DIR) $(DPDK_LIB_LIST) \
	-Wl,--no-whole-archive \
	-ldl -pthread -lrt -lrdmacm -lnuma -libverbs -luuid
