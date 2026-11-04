OTA_ROOTFS_SIZE ?= "3072"
OTA_DATA_SIZE ?= "128"
OTA_SWAP_SIZE ?= "512"

OVERLAYFS_MOUNT_POINT[data] ?= "/data"
OVERLAYFS_QA_SKIP[data] = "mount-configured"

# Discoverable Partitions Specification (DPS)
ROOT_DPS:arm = "69dad710-2ce4-4e3c-b16c-21a1d49abed3"
ROOT_DPS:aarch64 = "b921b045-1df0-41c3-af44-4c6f280d3fae"
SWAP_DPS = "0657fd6d-a4ab-43c4-84e5-0933c84b4f4f"