# Copyright 2025 NXP
# SPDX-License-Identifier: Apache-2.0

zephyr_library()

zephyr_library_sources(mchp_fusebit_programmer.c)

zephyr_linker_sources(SECTIONS userpage.ld)
