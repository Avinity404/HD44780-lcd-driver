/**
 * @file HD44780.c
 * @brief Implementation of the HD44780-compatible character LCD driver.
 */

#include "HD44780.h"

#define CMD_CLEAR        0x01U
#define CMD_RETURN_HOME  0x02U
#define CMD_DISP_OFF     0x08U
#define CMD_SET_DDRAM    0x80U
#define CMD_FUNCTION_SET 0x20U

#define HD44780_COMMAND     0U
#define HD44780_CHAR        1U

/* Function Set: DL = 0 selects the 4-bit interface. */
#define FUNC_TWO_LINES	 0x08U

static void HD44780_pulsEnable(hd44780_t* lcd)
{
	lcd->config.hal.write_pin(lcd->config.context, lcd->config.pins[HD44780_E], 1U);
	lcd->config.hal.delay_us(1U);
	lcd->config.hal.write_pin(lcd->config.context, lcd->config.pins[HD44780_E], 0U);
	lcd->config.hal.delay_us(1U);
}

static void HD44780_sendNibble(hd44780_t* lcd, uint8_t nibble)
{
	lcd->config.hal.write_pin(lcd->config.context, lcd->config.pins[HD44780_D4], (nibble >> 0U) & 1U);
	lcd->config.hal.write_pin(lcd->config.context, lcd->config.pins[HD44780_D5], (nibble >> 1U) & 1U);
	lcd->config.hal.write_pin(lcd->config.context, lcd->config.pins[HD44780_D6], (nibble >> 2U) & 1U);
	lcd->config.hal.write_pin(lcd->config.context, lcd->config.pins[HD44780_D7], (nibble >> 3U) & 1U);
	HD44780_pulsEnable(lcd);

}

static void HD44780_write(hd44780_t* lcd, uint8_t value, uint8_t mode)
{
	lcd->config.hal.write_pin(lcd->config.context, lcd->config.pins[HD44780_RS], mode);

	HD44780_sendNibble(lcd, value >> 4U);
	HD44780_sendNibble(lcd, value & 0x0FU);

	lcd->config.hal.delay_us(50U);
}


/* Ensure that every LCD signal has a distinct pin identifier. */
static hd44780_status_t HD44780_validatePins(const hd44780_Config_t* config)
{
	for (uint8_t i = 0U; i < HD44780_SIGNAL_COUNT; i++)
	{
		for (uint8_t j = i + 1U; j < HD44780_SIGNAL_COUNT; j++)
		{
			if (config->pins[i] == config->pins[j])
			{
				return HD44780_ERROR_ARGUMENT;
			}
		}
	}

	return HD44780_OK;
}

static hd44780_status_t HD44780_validateConfig(const hd44780_Config_t* config)
{
	if ( (config->hal.delay_us == NULL) || (config->hal.write_pin == NULL) )
	{
		return HD44780_ERROR_ARGUMENT;
	}

	if ( (config->cursorBlink > 1U) || (config->cursorShow > 1U) )
	{
		return HD44780_ERROR_ARGUMENT;
	}

	if ( (config->cols < 1U) || (config->cols > 40U) || (config->rows < 1U) || (config->rows > 4U) )
	{
		return HD44780_ERROR_ARGUMENT;
	}

	if (HD44780_validatePins(config) != HD44780_OK)
	{
		return HD44780_ERROR_ARGUMENT;
	}

	return HD44780_OK;
}



hd44780_status_t HD44780_InitInstance(hd44780_t* lcd, const hd44780_Config_t* config)
{
	if ( (lcd == NULL) || (config == NULL) )
	{
		return HD44780_ERROR_ARGUMENT;
	}

	if(HD44780_validateConfig(config) != HD44780_OK)
	{
		lcd->status = HD44780_ERROR_ARGUMENT;
		return HD44780_ERROR_ARGUMENT;
	}


	lcd->isInit = 0U;
    lcd->config = *config;

    lcd->config.hal.delay_us(50000U);

    lcd->config.hal.write_pin(lcd->config.context, lcd->config.pins[HD44780_RS], 0U);
    lcd->config.hal.write_pin(lcd->config.context, lcd->config.pins[HD44780_E], 0U);

    /* Reset sequence from the HD44780 datasheet. */
    HD44780_sendNibble(lcd, 0x03);
    lcd->config.hal.delay_us(5000U);
    HD44780_sendNibble(lcd, 0x03);
    lcd->config.hal.delay_us(200U);
    HD44780_sendNibble(lcd, 0x03);
    lcd->config.hal.delay_us(200U);

    HD44780_sendNibble(lcd, 0x02);
    lcd->config.hal.delay_us(200U);

    uint8_t functionSet = CMD_FUNCTION_SET;
	if (lcd->config.rows > 1U)
	{
		functionSet |= FUNC_TWO_LINES;
	}

	HD44780_write(lcd, functionSet, HD44780_COMMAND);


    HD44780_write(lcd, CMD_DISP_OFF, HD44780_COMMAND);
    HD44780_write(lcd, CMD_CLEAR, HD44780_COMMAND);
    lcd->config.hal.delay_us(2000U);

    HD44780_write(lcd, 0x06, HD44780_COMMAND);

    uint8_t displayControl = 0x0CU;
    if (lcd->config.cursorShow)
    {
    	displayControl |= 0x02U;
    }

    if (lcd->config.cursorBlink)
    {
    	displayControl |= 0x01U;
    }

    HD44780_write(lcd, displayControl, HD44780_COMMAND);

    lcd->config.hal.delay_us(100U);

    lcd->isInit = 1U;
    lcd->status = HD44780_OK;
    return HD44780_OK;
}


hd44780_status_t HD44780_DeInit(hd44780_t* lcd)
{
	if (lcd == NULL)
	{
		return HD44780_ERROR_ARGUMENT;
	}

	if (!lcd->isInit)
	{
		lcd->status = HD44780_ERROR_NOT_INIT;
		return HD44780_ERROR_NOT_INIT;
	}

	HD44780_write(lcd, CMD_DISP_OFF, HD44780_COMMAND);

	lcd->isInit = 0U;
	lcd->status = HD44780_OK;
	return HD44780_OK;
}


hd44780_status_t HD44780_ClearDisplay(hd44780_t* lcd)
{
	if (lcd == NULL)
	{
		return HD44780_ERROR_ARGUMENT;
	}

	if (!lcd->isInit)
	{
		lcd->status = HD44780_ERROR_NOT_INIT;
		return HD44780_ERROR_NOT_INIT;
	}

	HD44780_write(lcd, CMD_CLEAR, HD44780_COMMAND);
	lcd->config.hal.delay_us(2000U);

	lcd->status = HD44780_OK;
	return HD44780_OK;
}

hd44780_status_t HD44780_SetCursor(hd44780_t* lcd, uint8_t row, uint8_t col)
{
	if (lcd == NULL)
	{
		return HD44780_ERROR_ARGUMENT;
	}

	if (!lcd->isInit)
	{
		lcd->status = HD44780_ERROR_NOT_INIT;
		return HD44780_ERROR_NOT_INIT;
	}

	if (row >= lcd->config.rows || col >= lcd->config.cols)
	{
		lcd->status = HD44780_ERROR_ARGUMENT;
		return HD44780_ERROR_ARGUMENT;
	}

	uint8_t rowAddress[4];
	rowAddress[0] = 0x00U;
	rowAddress[1] = 0x40U;
	rowAddress[2] = lcd->config.cols;
	rowAddress[3] = 0x40U + lcd->config.cols;

	uint8_t addr = rowAddress[row] + col;
	HD44780_write(lcd, (CMD_SET_DDRAM | addr), HD44780_COMMAND);

	lcd->status = HD44780_OK;
	return HD44780_OK;
}

hd44780_status_t HD44780_PrintOnDisplay(hd44780_t* lcd, const char* s)
{
	if (lcd == NULL)
	{
		return HD44780_ERROR_ARGUMENT;
	}

	if (s == NULL)
	{
		lcd->status = HD44780_ERROR_ARGUMENT;
		return HD44780_ERROR_ARGUMENT;
	}

	if (!lcd->isInit)
	{
		lcd->status = HD44780_ERROR_NOT_INIT;
		return HD44780_ERROR_NOT_INIT;
	}

	while (*s != '\0')
	{
		HD44780_write(lcd, (uint8_t)*s, HD44780_CHAR);
		s++;
	}

	lcd->status = HD44780_OK;
	return HD44780_OK;
}

hd44780_status_t HD44780_ReturnHome(hd44780_t* lcd)
{
	if (lcd == NULL)
	{
		return HD44780_ERROR_ARGUMENT;
	}

	if (!lcd->isInit)
	{
		lcd->status = HD44780_ERROR_NOT_INIT;
		return HD44780_ERROR_NOT_INIT;
	}

	HD44780_write(lcd, CMD_RETURN_HOME, HD44780_COMMAND);
	lcd->config.hal.delay_us(2000U);

	lcd->status = HD44780_OK;
	return HD44780_OK;
}

