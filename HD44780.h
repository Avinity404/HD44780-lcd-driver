/**
 * @file HD44780.h
 * @brief Public interface for an HD44780-compatible character LCD driver.
 */

#ifndef HD44780_H_
#define HD44780_H_

#include <stdint.h>
#include <stddef.h>

/** Logical LCD signals used to index the configured pin array. */
typedef enum
{
    HD44780_RS = 0,
    HD44780_E,
    HD44780_D4,
    HD44780_D5,
    HD44780_D6,
    HD44780_D7,
    HD44780_SIGNAL_COUNT
} hd44780_Signal_t;

/** Status codes returned by the driver API. */
typedef enum {
	HD44780_OK = 0,
	HD44780_ERROR_ARGUMENT,
	HD44780_ERROR_NOT_INIT
} hd44780_status_t;

/**
 * Platform callbacks required by the driver.
 * The LCD interface is write-only, so no read callback is required.
 */
typedef struct
{
    void (*write_pin)(void* context, uint8_t pin, uint8_t state);
    void (*delay_us)(uint32_t us);
} hd44780_hal_t;

/** Configuration for one LCD instance. */
typedef struct
{
    uint8_t rows;
    uint8_t cols;
    uint8_t cursorShow;
	uint8_t cursorBlink;

	/** Pin identifiers indexed by hd44780_Signal_t. */
    uint8_t pins[HD44780_SIGNAL_COUNT];

    hd44780_hal_t hal;

    /** Opaque value passed to the write_pin callback. */
    void* context;
} hd44780_Config_t;

/** Runtime state for one LCD instance. Initialize to zero before use. */
typedef struct
{
    hd44780_Config_t config;
    hd44780_status_t status;
    uint8_t isInit;
} hd44780_t;


hd44780_status_t HD44780_InitInstance(hd44780_t* lcd, const hd44780_Config_t* config);
hd44780_status_t HD44780_DeInit(hd44780_t* lcd);
hd44780_status_t HD44780_ClearDisplay(hd44780_t* lcd);
hd44780_status_t HD44780_SetCursor(hd44780_t* lcd, uint8_t row, uint8_t col);
hd44780_status_t HD44780_PrintOnDisplay(hd44780_t* lcd, const char* s);
hd44780_status_t HD44780_ReturnHome(hd44780_t* lcd);

#endif /* HD44780_H_ */
