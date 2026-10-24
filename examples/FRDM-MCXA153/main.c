/*
 * Copyright 2016-2026 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file    main.c
 * @brief   HD44780 LCD driver example for FRDM-MCXA153
 */

#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"

#include "HD44780.h"
#include "nxp_hal.h"

hd44780_Config_t config = {
		.rows = 2,
		.cols = 16,
		.cursorShow = 0,
		.cursorBlink = 0,

		.pins = {
			[HD44780_RS] = 11,
			[HD44780_E]  = 10,
			[HD44780_D4] = 9,
			[HD44780_D5] = 8,
			[HD44780_D6] = 7,
			[HD44780_D7] = 6
		},

		.hal = {
			.write_pin = nxp_write_pin,
			.delay_us = nxp_delay_us
		},

		.context = GPIO3
};

hd44780_t lcd = {0};
char sbuff[17];
uint16_t val = 0;

int main(void)
{
    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif

    PRINTF("Run\r\n");

    if( HD44780_InitInstance(&lcd, &config) != HD44780_OK)
    {
    	PRINTF("HD44780 initialization failed!\r\n");

    	while(1)
    	{

    	}
    }

    HD44780_PrintOnDisplay(&lcd, "Hello HD44780!");

    while(1)
    {
    	snprintf(sbuff, sizeof(sbuff), "val: %u", (unsigned int)val);

    	HD44780_SetCursor(&lcd, 1, 0);
    	HD44780_PrintOnDisplay(&lcd, sbuff);

    	val++;

    	SDK_DelayAtLeastUs(100000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
    }

    return 0 ;
}
