/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 STMicroelectronics
 * SPDX-License-Identifier: Apache-2.0
 *
 * Driver for one-time programmable areas inside STM32 embedded NVM.
 *
 * "OTP for user data" area programming is not supported yet.
 * It shall be implemented as a dedicated, externally visible function
 * in the flash driver(s) called from this driver.
 */

#include <string.h>
#include <zephyr/cache.h>
#include <zephyr/device.h>
#include <zephyr/drivers/otp.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/math_extras.h>
#include <zephyr/sys/util.h>
#include <zephyr/toolchain.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/drivers/flash/mchp_flash.h>
#define DT_DRV_COMPAT microchip_nvm_g1_otp

struct otp_mchp_nvm_config {
	/* Base address and size of OTP area */
	const uint8_t *base;
	size_t size;
};

static void slow_otp_readout(void *buf, const uint8_t *src, size_t len)
{
	uintptr_t dst = (uintptr_t)buf;
	size_t remaining = len;
	uint16_t tmp;

	if (!IS_ALIGNED(src, sizeof(uint16_t))) {
		/*
		 * First byte is not aligned: read the entire halfword
		 * but write only the byte at higher address (which is
		 * the MSB due to STM32 CPUs being little-endian).
		 */
		tmp = sys_read16((mem_addr_t)(src - 1));
		sys_write8(tmp >> 8u, dst);
		src++;
		dst++;
		remaining--;
	}

	__ASSERT_NO_MSG(IS_ALIGNED(src, sizeof(uint16_t)));

	/* Copy bulk using 16-bit reads */
	while (remaining >= 2) {
		UNALIGNED_PUT(sys_read16((mem_addr_t)src), (uint16_t *)dst);
		src += sizeof(uint16_t);
		dst += sizeof(uint16_t);
		remaining -= sizeof(uint16_t);
	}

	__ASSERT_NO_MSG(remaining < 2);

	if (remaining == 1) {
		/*
		 * Ditto as above but for unaligned last byte.
		 * src was incremented at the end of the loop
		 * and already points to the target halfword;
		 * however, we keep the LSB this time since
		 * we want the byte at lower address.
		 */
		tmp = sys_read16((mem_addr_t)src);
		sys_write8(tmp & 0xFFu, dst);
	}
}

static int mchp_nvm_read(const struct device *dev, off_t offset, void *buf, size_t len)
{
	const struct otp_mchp_nvm_config *config = dev->config;
	const size_t start = (size_t)offset;
	size_t end;

	if (size_add_overflow(start, len, &end) || end > config->size) {
		return -EINVAL;
	}

	/*
	 * The OTP/RO area is mapped via the AHB interface which does not
	 * support 8-bit reads on series such as STM32H5 or STM32H7R/S.
	 *
	 * Do NOT use memcpy() - instead, copy using only 16-bit reads
	 * which should have the broadest compatibility.
	 */
	if (likely(IS_ALIGNED(start, sizeof(uint16_t)) && IS_ALIGNED(len, sizeof(uint16_t)))) {
		/* No unaligned head/tail to handle */
		const uint8_t *base = config->base;
		const char *dst = buf;
		size_t cur = start;

		while (cur < end) {
			uint16_t v = sys_read16((mem_addr_t)&base[cur]);

			UNALIGNED_PUT(v, (uint16_t *)dst);

			cur += sizeof(uint16_t);
			dst += sizeof(uint16_t);
		}

		__ASSERT_NO_MSG(cur == end);
	} else {
		slow_otp_readout(buf, &config->base[start], len);
	}

	return 0;
}

#define NVMCTRL_USERROW_PAGESIZE      (512U)
#define NVMCTRL_USERROW_START_ADDRESS (0x00804000U)
uint8_t read_array_userpage[NVMCTRL_USERROW_PAGESIZE];
uint8_t write_array_userpage[NVMCTRL_USERROW_PAGESIZE];
uint8_t wr_userpage_defaultvalues[16] = {0x39, 0x92, 0x9a, 0xfe,
					 0x80, 0xff, 0xa8, 0xaa,
					 0xff, 0xff, 0xff, 0xff,
					0xff, 0xff, 0xff, 0xff};

void populate_buffer(uint8_t *data, uint32_t size)
{
	int i = 0;

	for (i = 0; i < size; i++) {
		*(data + i) = i;
	}
}
//read complete bytes to local something
//update required fields based on devicetree entries?
//erase teh complete page (512 bytes)
//write back the contents
static int mchp_nvm_write(const struct device *dev, off_t offset, const void *buf, size_t len)
{
	const struct otp_mchp_nvm_config *config = dev->config;
	const struct device *nvmctrl_dev = DEVICE_DT_GET(DT_NODELABEL(nvmctrl));
	{
		printf("flash_userpage_erase\n");

		uint16_t code = FLASH_EX_OP_USER_ROW_ERASE;
		uint8_t out;
		uint8_t in;

		populate_buffer(read_array_userpage, sizeof(read_array_userpage));

		memcpy(read_array_userpage, (uint8_t *)NVMCTRL_USERROW_START_ADDRESS,
		       NVMCTRL_USERROW_PAGESIZE);

		for (int i = 0; i < NVMCTRL_USERROW_PAGESIZE; i++) {
			printf("UserPage_Memory[0x%X] 0x%x ", i + (NVMCTRL_USERROW_START_ADDRESS),
			       read_array_userpage[i]);
		}

		int ret = flash_ex_op(nvmctrl_dev, code, (const uintptr_t)&in, &out);
		printf("flash_ex_op ret val :  %d\n", ret);

		memcpy(read_array_userpage, (uint8_t *)NVMCTRL_USERROW_START_ADDRESS,
		       NVMCTRL_USERROW_PAGESIZE);

		for (int i = 0; i < NVMCTRL_USERROW_PAGESIZE; i++) {
			printf("UserPage_Memory[0x%X] 0x%x ", i + (NVMCTRL_USERROW_START_ADDRESS),
			       read_array_userpage[i]);
		}
	}

	uint16_t code = FLASH_EX_OP_USER_ROW_WRITE;
	uint8_t out;
	struct flash_mchp_ex_op_userrow_data up_in = {
		.data = wr_userpage_defaultvalues, .data_len = 16, .offset = 0};

	populate_buffer(write_array_userpage, sizeof(write_array_userpage));

	int ret = flash_ex_op(nvmctrl_dev, code, (const uintptr_t)&up_in, &out);
	printf("flash_ex_op ret val :  %d\n", ret);

	memcpy(read_array_userpage, (uint8_t *)NVMCTRL_USERROW_START_ADDRESS,
	       NVMCTRL_USERROW_PAGESIZE);

	for (int i = 0; i < NVMCTRL_USERROW_PAGESIZE; i++) {
		printf("UserPage_Memory[0x%X] 0x%x \n", i + (NVMCTRL_USERROW_START_ADDRESS),
		       read_array_userpage[i]);
	}

	return 0;
}

static DEVICE_API(otp, otp_mchp_nvm_api) = {
	.read = mchp_nvm_read,
	.program = mchp_nvm_write,
};

#define OTP_STM32_NVM_INIT_INNER(inst, _cfg)                                                       \
	static const struct otp_mchp_nvm_config _cfg = {                                           \
		.base = (void *)DT_INST_REG_ADDR(inst),                                            \
		.size = DT_INST_REG_SIZE(inst),                                                    \
	};                                                                                         \
                                                                                                   \
	DEVICE_DT_INST_DEFINE(inst, NULL, NULL, NULL, &_cfg, POST_KERNEL,                          \
			      CONFIG_OTP_INIT_PRIORITY, &otp_mchp_nvm_api);

#define OTP_STM32_NVM_INIT(inst) OTP_STM32_NVM_INIT_INNER(inst, CONCAT(otp_mchp_nvm_cfg, inst))

DT_INST_FOREACH_STATUS_OKAY(OTP_STM32_NVM_INIT)
