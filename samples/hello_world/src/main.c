/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
__attribute__((section(".userpage")))
const unsigned char user_nvm_data[256] = {
    0xDE, 0xAD, 0xBE, 0xEF
};

#include <stdio.h>

int main(void)
{
	printf("Hello World! %s\n", CONFIG_BOARD_TARGET);

	return 0;
}
