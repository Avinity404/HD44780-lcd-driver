/**
 * @file nxp_hal.h
 * @brief Hardware abstraction layer for the NXP FRDM-MCXA153 example.
 */

#ifndef NXP_HAL_H_
#define NXP_HAL_H_

#include <stdint.h>

void nxp_write_pin(void* context, uint8_t pin, uint8_t state);
void nxp_delay_us(uint32_t us);


#endif /* NXP_HAL_H_ */
