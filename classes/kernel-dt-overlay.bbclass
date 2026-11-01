#
# Device tree overlay support
#
# Overlays are compiled and merged into the DTBs listed in KERNEL_DEVICETREE
# during the build, so a single complete <base>.dtb is installed and deployed.
# No bootloader or extlinux changes are required, and no standalone .dtbo is
# shipped.
#
# KERNEL_DT_OVERLAYS
#     Space-separated overlay sources, relative to ${UNPACKDIR}.  Each source
#     is copied into arch/${ARCH}/boot/dts/rockchip and compiled there; its
#     basename (minus .dtso) is the overlay name used for the resulting .dtbo.
#     Keep the .dtso suffix, because that is the only suffix the kernel knows
#     how to turn into a .dtbo.
#
#     Every overlay is applied, in the listed order, to each DTB in
#     KERNEL_DEVICETREE.
#
# The variable is empty by default, so machines without overlays are unaffected
# (the task degrades to a no-op).
#
# Usage:
#   inherit kernel-dt-overlay
#   KERNEL_DT_OVERLAYS:append:my-board = " overlays/my-board-uart.dtso"
#
# The class must be inherited by a kernel recipe (it relies on the kernel
# build variables and on ${S}/${B} being the kernel source/build trees).
#

KERNEL_DT_OVERLAYS ?= ""

# Overlays reference labels, so the base DTBs must export __symbols__ (-@).
python __anonymous() {
    if d.getVar("KERNEL_DT_OVERLAYS"):
        d.appendVar("KERNEL_DTC_FLAGS", " -@")
}

# Copy overlay sources into the kernel tree.  Kept as a helper because the
# shared kernel source can be refreshed between do_configure and the overlay
# tasks.
kernel_dt_install_overlays() {
	for ovl in ${KERNEL_DT_OVERLAYS}; do
		install -Dm 0644 ${UNPACKDIR}/${ovl} \
			${S}/arch/${ARCH}/boot/dts/rockchip/$(basename ${ovl})
	done
}

do_configure:append() {
	kernel_dt_install_overlays
}

do_overlay_apply() {
	if [ -z "${KERNEL_DT_OVERLAYS}" ]; then
		bbnote "No device tree overlay configured for ${MACHINE}"
		return 0
	fi

	dtb_dir="${B}/arch/${ARCH}/boot/dts/rockchip"

	kernel_dt_install_overlays

	# Build every overlay once; the same set is applied to each base DTB.
	overlay_paths=""
	for src in ${KERNEL_DT_OVERLAYS}; do
		overlay="$(basename ${src} .dtso)"

		oe_runmake -C ${S} O=${B} rockchip/${overlay}.dtbo \
			CC="${KERNEL_CC} $cc_extra" LD="${KERNEL_LD}" \
			OBJCOPY="${KERNEL_OBJCOPY}" STRIP="${KERNEL_STRIP}" \
			${KERNEL_EXTRA_ARGS}

		[ -e "${dtb_dir}/${overlay}.dtbo" ] || \
			bbfatal "failed to build device tree overlay ${overlay}"

		overlay_paths="${overlay_paths} ${dtb_dir}/${overlay}.dtbo"
	done

	for dtb in ${KERNEL_DEVICETREE}; do
		base_dtb="$(basename ${dtb} .dtb)"
		base="${dtb_dir}/${base_dtb}.dtb"

		[ -e "${base}" ] || \
			bbfatal "missing base device tree ${base}"

		bbnote "Applying overlays to ${base_dtb}.dtb:${KERNEL_DT_OVERLAYS}"
		${B}/scripts/dtc/fdtoverlay -i "${base}" \
			-o ${D}/${KERNEL_DTBDEST}/${base_dtb}.dtb \
			${overlay_paths}
	done
}

addtask do_overlay_apply after do_install before do_package do_deploy
