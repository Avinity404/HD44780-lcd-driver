/**
 * @file nxp_hal.h
 * @brief Hardware abstraction layer for the NXP FRDM-MCXA153 example.
 */

#include "nxp_hal.h"
#include <stdio.h>
#include "board.h"
#include "pin_mux.h"


void nxp_write_pin(void* context, uint8_t pin, uint8_t state)
{
    GPIO_Type *gpio = (GPIO_Type*)context;
    if (gpio == NULL)
    {
    	return;
    }
    GPIO_PinWrite(gpio, pin, state);
}

void nxp_delay_us(uint32_t us)
{
    SDK_DelayAtLeastUs(us, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
}
