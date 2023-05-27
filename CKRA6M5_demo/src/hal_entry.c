/***********************************************************************************************************************
 * Copyright [2020-2022] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
 *
 * This software and documentation are supplied by Renesas Electronics America Inc. and may only be used with products
 * of Renesas Electronics Corp. and its affiliates ("Renesas").  No other uses are authorized.  Renesas products are
 * sold pursuant to Renesas terms and conditions of sale.  Purchasers are solely responsible for the selection and use
 * of Renesas products and Renesas assumes no liability.  No license, express or implied, to any intellectual property
 * right is granted by Renesas. This software is protected under all applicable laws, including copyright laws. Renesas
 * reserves the right to change or discontinue this software and/or this documentation. THE SOFTWARE AND DOCUMENTATION
 * IS DELIVERED TO YOU "AS IS," AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND TO THE FULLEST EXTENT
 * PERMISSIBLE UNDER APPLICABLE LAW, DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY, INCLUDING WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE SOFTWARE OR
 * DOCUMENTATION.  RENESAS SHALL HAVE NO LIABILITY ARISING OUT OF ANY SECURITY VULNERABILITY OR BREACH.  TO THE MAXIMUM
 * EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE OR DOCUMENTATION
 * (OR ANY PERSON OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER, INCLUDING,
 * WITHOUT LIMITATION, ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES; ANY LOST PROFITS,
 * OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH LOSS, DAMAGES, CLAIMS OR COSTS.
 **********************************************************************************************************************/

#include "hal_data.h"
#include <stdio.h>

void R_BSP_WarmStart(bsp_warm_start_event_t event);

void user_uart_callback(uart_callback_args_t *p_args);
void console_write(const char *buffer);
static volatile bool is_transfer_complete = false;

void g_comms_i2c_bus0_quick_setup(void);

void g_hs300x_sensor0_quick_setup(void);
void g_hs300x_sensor0_quick_getting_humidity_and_temperature(rm_hs300x_data_t *p_data);

void g_zmod4xxx_sensor0_quick_setup(void);
void g_zmod4xxx_sensor0_quick_getting_iaq_1st_gen_continuous_mode_data(rm_zmod4xxx_iaq_1st_data_t *p_gas_data);

void g_ob1203_sensor0_quick_setup(void);
void g_ob1203_sensor0_quick_getting_light_mode_data(rm_ob1203_light_data_t *p_ob1203_data,
        rm_ob1203_light_data_type_t data_type);
void g_ob1203_sensor0_quick_getting_proximity_mode_data(rm_ob1203_prox_data_t *p_ob1203_data);

extern bsp_leds_t g_bsp_leds;

/*******************************************************************************************************************//**
 * @brief  Blinky example application
 *
 * Blinks all leds at a rate of 1 second using the software delay function provided by the BSP.
 *
 **********************************************************************************************************************/
void hal_entry(void)
{
#if BSP_TZ_SECURE_BUILD

    /* Enter non-secure code */
    R_BSP_NonSecureEnter();
#endif

    /* Define the units to be used with the software delay function */
    const bsp_delay_units_t bsp_delay_units = BSP_DELAY_UNITS_MILLISECONDS;

    /* Set the blink frequency (must be <= bsp_delay_units */
    const uint32_t freq_in_hz = 2;

    /* Calculate the delay in terms of bsp_delay_units */
    const uint32_t delay = bsp_delay_units / freq_in_hz;

    /* LED type structure */
    bsp_leds_t leds = g_bsp_leds;

    /* If this board has no LEDs then trap here */
    if (0 == leds.led_count)
    {
        while (1)
        {
            ;                          // There are no LEDs on this board
        }
    }

    /* Holds level to set for pins */
    bsp_io_level_t pin_level = BSP_IO_LEVEL_LOW;

    char write_buffer[200] =
    { };
    /* Initialize UART */
    R_SCI_UART_Open (&g_console_write_ctrl, &g_console_write_cfg);
    // sprintf (write_buffer, "Hello world!\r\n");

    g_comms_i2c_bus0_quick_setup ();

    g_hs300x_sensor0_quick_setup ();

    g_zmod4xxx_sensor0_quick_setup ();

    g_ob1203_sensor0_quick_setup ();

    while (1)
    {
        /* Enable access to the PFS registers. If using r_ioport module then register protection is automatically
         * handled. This code uses BSP IO functions to show how it is used.
         */
        R_BSP_PinAccessEnable ();

        /* Update all board LEDs */
        for (uint32_t i = 0; i < leds.led_count; i++)
        {
            /* Get pin to toggle */
            uint32_t pin = leds.p_leds[i];

            /* Write to this pin */
            R_BSP_PinWrite ((bsp_io_port_pin_t) pin, pin_level);
        }

        /* Protect PFS registers */
        R_BSP_PinAccessDisable ();

        /* Toggle level for next write */
        if (BSP_IO_LEVEL_LOW == pin_level)
        {
            pin_level = BSP_IO_LEVEL_HIGH;
        }
        else
        {
            pin_level = BSP_IO_LEVEL_LOW;
        }

        /* HS3001 DATA */
        rm_hs300x_data_t p_data;

        g_hs300x_sensor0_quick_getting_humidity_and_temperature (&p_data);

        sprintf (write_buffer, "HS3001:\n\r");
        console_write (write_buffer);

        sprintf (write_buffer, "Humidity value: %d.%d \n\r", p_data.humidity.integer_part,
                 p_data.humidity.decimal_part);
        console_write (write_buffer);

        sprintf (write_buffer, "Temperature value: %d.%d \n\r", p_data.temperature.integer_part,
                 p_data.temperature.decimal_part);
        console_write (write_buffer);

        /* ZMOD4410 DATA */
        rm_zmod4xxx_iaq_1st_data_t p_gas_data;

        g_zmod4xxx_sensor0_quick_getting_iaq_1st_gen_continuous_mode_data (&p_gas_data);

        sprintf (write_buffer, "ZMOD4410:\n\r");
        console_write (write_buffer);

        float num = p_gas_data.rmox;
        int integer_part = (int) num;
        int decimal_part = (int) ((num - integer_part) * 10);
        sprintf (write_buffer, "rmox: %d.%d \n\r", integer_part, decimal_part);
        console_write (write_buffer);

        num = p_gas_data.rcda;
        integer_part = (int) num;
        decimal_part = (int) ((num - integer_part) * 10);
        sprintf (write_buffer, "rcda: %d.%d \n\r", integer_part, decimal_part);
        console_write (write_buffer);

        num = p_gas_data.iaq;
        integer_part = (int) num;
        decimal_part = (int) ((num - integer_part) * 10);
        sprintf (write_buffer, "iaq: %d.%d \n\r", integer_part, decimal_part);
        console_write (write_buffer);

        num = p_gas_data.tvoc;
        integer_part = (int) num;
        decimal_part = (int) ((num - integer_part) * 10);
        sprintf (write_buffer, "tvoc: %d.%d \n\r", integer_part, decimal_part);
        console_write (write_buffer);

        num = p_gas_data.etoh;
        integer_part = (int) num;
        decimal_part = (int) ((num - integer_part) * 10);
        sprintf (write_buffer, "etoh: %d.%d \n\r", integer_part, decimal_part);
        console_write (write_buffer);

        num = p_gas_data.eco2;
        integer_part = (int) num;
        decimal_part = (int) ((num - integer_part) * 10);
        sprintf (write_buffer, "eco2: %d.%d \n\r", integer_part, decimal_part);
        console_write (write_buffer);

        /* OB1203 DATA */
        rm_ob1203_light_data_t p_ob1203_data;
        rm_ob1203_light_data_type_t data_type = RM_OB1203_LIGHT_DATA_TYPE_ALL;
        g_ob1203_sensor0_quick_getting_light_mode_data (&p_ob1203_data, data_type);

        sprintf (write_buffer, "OB1203 LIGHT:\n\r");
        console_write (write_buffer);

        sprintf (write_buffer, "clear_data: %lu \n\r", p_ob1203_data.clear_data);
        console_write (write_buffer);
        sprintf (write_buffer, "green_data: %lu \n\r", p_ob1203_data.green_data);
        console_write (write_buffer);
        sprintf (write_buffer, "blue_data: %lu \n\r", p_ob1203_data.blue_data);
        console_write (write_buffer);
        sprintf (write_buffer, "red_data: %lu \n\r", p_ob1203_data.red_data);
        console_write (write_buffer);
        sprintf (write_buffer, "comp_data: %lu \n\r", p_ob1203_data.comp_data);
        console_write (write_buffer);

        rm_ob1203_prox_data_t p_ob1203_data_2;
        g_ob1203_sensor0_quick_getting_proximity_mode_data (&p_ob1203_data_2);

        sprintf (write_buffer, "OB1203 Proximity:\n\r");
        console_write (write_buffer);

        sprintf (write_buffer, "proximity_data: %u \n\r", p_ob1203_data_2.proximity_data);
        console_write (write_buffer);

        /* Delay */
        R_BSP_SoftwareDelay (delay, bsp_delay_units);

        /* Send clear screen message & cursor home command */
        sprintf (write_buffer, "\x1b[H");
        console_write (write_buffer);
    }
}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_RESET == event)
    {
#if BSP_FEATURE_FLASH_LP_VERSION != 0

        /* Enable reading from data flash. */
        R_FACI_LP->DFLCTL = 1U;

        /* Would normally have to wait tDSTOP(6us) for data flash recovery. Placing the enable here, before clock and
         * C runtime initialization, should negate the need for a delay since the initialization will typically take more than 6us. */
#endif
    }

    if (BSP_WARM_START_POST_C == event)
    {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open (&g_ioport_ctrl, g_ioport.p_cfg);
    }
}

void user_uart_callback(uart_callback_args_t *p_args)
{
    switch (p_args->event)
    {
        case UART_EVENT_TX_COMPLETE:
        {
            is_transfer_complete = true;
            break;
        }
        default:
        {
            /* Do nothing */
            break;
        }
    }
}

void console_write(const char *buffer)
{
    is_transfer_complete = false;

    R_SCI_UART_Write (&g_console_write_ctrl, (uint8_t*) buffer, strlen (buffer));

    while (!is_transfer_complete)
    {
        R_BSP_SoftwareDelay (1, BSP_DELAY_UNITS_MICROSECONDS);
    }
}

/* TODO: Enable if you want to open I2C bus */

/* Quick setup for i2c_bus_name. */
void g_comms_i2c_bus0_quick_setup(void)
{
    fsp_err_t err;
    i2c_master_instance_t *p_driver_instance = (i2c_master_instance_t*) g_comms_i2c_bus0_extended_cfg.p_driver_instance;

    /* Open I2C driver, this must be done before calling any COMMS API */
    err = p_driver_instance->p_api->open (p_driver_instance->p_ctrl, p_driver_instance->p_cfg);
    assert(FSP_SUCCESS == err);
}

/* TODO: Enable if you want to open ZMOD4XXX */
#define G_ZMOD4XXX_SENSOR0_NON_BLOCKING 1
#define G_ZMOD4XXX_SENSOR0_IRQ_ENABLE   0

#if G_ZMOD4XXX_SENSOR0_NON_BLOCKING
volatile bool g_zmod4xxx_i2c_completed = false;
volatile rm_zmod4xxx_event_t g_zmod4xxx_i2c_callback_event;
#endif
#if G_ZMOD4XXX_SENSOR0_IRQ_ENABLE
volatile bool g_zmod4xxx_irq_completed = false;
#endif

/* TODO: Enable if you want to use a I2C callback */
#define G_ZMOD4XXX_SENSOR0_I2C_CALLBACK_ENABLE 1

#if G_ZMOD4XXX_SENSOR0_I2C_CALLBACK_ENABLE
void zmod4xxx_comms_i2c_callback(rm_zmod4xxx_callback_args_t *p_args)
{
#if G_ZMOD4XXX_SENSOR0_NON_BLOCKING
    g_zmod4xxx_i2c_callback_event = p_args->event;

    if (RM_ZMOD4XXX_EVENT_ERROR != p_args->event)
    {
        g_zmod4xxx_i2c_completed = true;
    }
#else
    FSP_PARAMETER_NOT_USED(p_args);
#endif
}
#endif

/* TODO: Enable if you want to use a IRQ callback */
#define G_ZMOD4XXX_SENSOR0_IRQ_CALLBACK_ENABLE 0

#if G_ZMOD4XXX_SENSOR0_IRQ_CALLBACK_ENABLE
void zmod4xxx_irq_callback(rm_zmod4xxx_callback_args_t *p_args)
{
#if G_ZMOD4XXX_SENSOR0_IRQ_ENABLE
    if (RM_ZMOD4XXX_EVENT_MEASUREMENT_COMPLETE == p_args->event)
    {
        g_zmod4xxx_irq_completed = true;
    }
#else
    FSP_PARAMETER_NOT_USED(p_args);
#endif
}
#endif

/* Quick setup for zmod4xxx_name.
 * - i2c_bus_name must be setup before calling this function
 *     (See Developer Assistance -> zmod4xxx_name -> ZMOD4xxx ***** on rm_zmod4xxx -> i2c_device_name -> i2c_bus_name -> Quick Setup).
 */

/* Quick setup for zmod4xxx_name. */
void g_zmod4xxx_sensor0_quick_setup(void)
{
    fsp_err_t err;

    /* Reset ZMOD sensor (active low). Please change to the IO port connected to the RES_N pin of the ZMOD sensor on the customer board. */
    R_IOPORT_PinWrite (&g_ioport_ctrl, BSP_IO_PORT_03_PIN_07, BSP_IO_LEVEL_HIGH);
    R_BSP_SoftwareDelay (10, BSP_DELAY_UNITS_MILLISECONDS);
    R_IOPORT_PinWrite (&g_ioport_ctrl, BSP_IO_PORT_03_PIN_07, BSP_IO_LEVEL_LOW);
    R_BSP_SoftwareDelay (10, BSP_DELAY_UNITS_MILLISECONDS);
    R_IOPORT_PinWrite (&g_ioport_ctrl, BSP_IO_PORT_03_PIN_07, BSP_IO_LEVEL_HIGH);
    R_BSP_SoftwareDelay (10, BSP_DELAY_UNITS_MILLISECONDS);

    /* Open ZMOD4XXX sensor instance, this must be done before calling any ZMOD4XXX API */
    err = g_zmod4xxx_sensor0.p_api->open (g_zmod4xxx_sensor0.p_ctrl, g_zmod4xxx_sensor0.p_cfg);
    assert(FSP_SUCCESS == err);
}

/* Quick getting IAQ 1st Gen. Continuous mode values for zmod4xxx_name.
 * - zmod4xxx_name must be setup before calling this function.
 */

/* Quick getting gas data for zmod4xxx_name. */
void g_zmod4xxx_sensor0_quick_getting_iaq_1st_gen_continuous_mode_data(rm_zmod4xxx_iaq_1st_data_t *p_gas_data)
{
    fsp_err_t err;
    rm_zmod4xxx_raw_data_t zmod4xxx_raw_data;
    bool in_stabilization = false;

    /* Clear callback flags */
#if G_ZMOD4XXX_SENSOR0_IRQ_ENABLE
    g_zmod4xxx_irq_completed = false;
#endif
#if G_ZMOD4XXX_SENSOR0_NON_BLOCKING
    g_zmod4xxx_i2c_completed = false;
#endif

    /* Start the measurement */
    /* If the MeasurementStart API is called once, a second call is not required. */
    err = g_zmod4xxx_sensor0.p_api->measurementStart (g_zmod4xxx_sensor0.p_ctrl);
    assert(FSP_SUCCESS == err);
#if G_ZMOD4XXX_SENSOR0_NON_BLOCKING
    while (!g_zmod4xxx_i2c_completed)
    {
        ;
    }
    g_zmod4xxx_i2c_completed = false;
#endif

    do
    {
        do
        {
            /* Wait for the measurement to complete */
#if G_ZMOD4XXX_SENSOR0_IRQ_ENABLE
            while (!g_zmod4xxx_irq_completed)
            {
                ;
            }
            g_zmod4xxx_irq_completed = false;
#else
            err = g_zmod4xxx_sensor0.p_api->statusCheck (g_zmod4xxx_sensor0.p_ctrl);
            assert(FSP_SUCCESS == err);
#if G_ZMOD4XXX_SENSOR0_NON_BLOCKING
            while (!g_zmod4xxx_i2c_completed)
            {
                ;
            }
            g_zmod4xxx_i2c_completed = false;
#endif
#endif
            /* Read ADC data from ZMOD4xxx sensor */
            err = g_zmod4xxx_sensor0.p_api->read (g_zmod4xxx_sensor0.p_ctrl, &zmod4xxx_raw_data);
            if (err == FSP_ERR_SENSOR_MEASUREMENT_NOT_FINISHED)
            {
                R_BSP_SoftwareDelay (50, BSP_DELAY_UNITS_MILLISECONDS);
            }
        }
        while (err == FSP_ERR_SENSOR_MEASUREMENT_NOT_FINISHED);
        assert(FSP_SUCCESS == err);

#if G_ZMOD4XXX_SENSOR0_NON_BLOCKING
        while (!g_zmod4xxx_i2c_completed)
        {
            ;
        }
        g_zmod4xxx_i2c_completed = false;
#endif

        /* Calculate IAQ 1st Gen. values from ZMOD4xxx ADC data */
        err = g_zmod4xxx_sensor0.p_api->iaq1stGenDataCalculate (g_zmod4xxx_sensor0.p_ctrl, &zmod4xxx_raw_data,
                                                                p_gas_data);
        if (err == FSP_SUCCESS)
        {
            in_stabilization = false;
        }
        else if (err == FSP_ERR_SENSOR_IN_STABILIZATION)
        {
            in_stabilization = true;
        }
        else
        {
            assert(false);
        }
    }
    while (true == in_stabilization);
}

/* TODO: Enable if you want to open HS300X */
#define G_HS300X_SENSOR0_NON_BLOCKING (1)

#if G_HS300X_SENSOR0_NON_BLOCKING
volatile bool g_hs300x_completed = false;
#endif

/* TODO: Enable if you want to use a callback */
#define G_HS300X_SENSOR0_CALLBACK_ENABLE (1)
#if G_HS300X_SENSOR0_CALLBACK_ENABLE
void hs300x_callback(rm_hs300x_callback_args_t *p_args)
{
#if G_HS300X_SENSOR0_NON_BLOCKING
    if (RM_HS300X_EVENT_SUCCESS == p_args->event)
    {
        g_hs300x_completed = true;
    }
#else
    FSP_PARAMETER_NOT_USED(p_args);
#endif
}
#endif

/* Quick setup for hs300x_name.
 * - i2c_bus_name must be setup before calling this function
 *     (See Developer Assistance -> hs300x_name -> i2c_device_name -> i2c_bus_name -> Quick Setup).
 */

/* Quick setup for hs300x_name. */
void g_hs300x_sensor0_quick_setup(void)
{
    fsp_err_t err;

    /* Open HS300X sensor instance, this must be done before calling any HS300X API */
    err = g_hs300x_sensor0.p_api->open (g_hs300x_sensor0.p_ctrl, g_hs300x_sensor0.p_cfg);
    assert(FSP_SUCCESS == err);
}

/* Quick getting humidity and temperature values for hs300x_name.
 * - hs300x_name must be setup before calling this function.
 */

/* Quick getting humidity and temperature for hs300x_name. */
void g_hs300x_sensor0_quick_getting_humidity_and_temperature(rm_hs300x_data_t *p_data)
{
    fsp_err_t err;
    rm_hs300x_raw_data_t hs300x_raw_data;
    bool is_valid_data = false;

    /* Start the measurement */
    err = g_hs300x_sensor0.p_api->measurementStart (g_hs300x_sensor0.p_ctrl);
    assert(FSP_SUCCESS == err);
#if G_HS300X_SENSOR0_NON_BLOCKING
    while (!g_hs300x_completed)
    {
        ;
    }
    g_hs300x_completed = false;
#endif

    do
    {
        /* Read ADC data from HS300X sensor */
        err = g_hs300x_sensor0.p_api->read (g_hs300x_sensor0.p_ctrl, &hs300x_raw_data);
        assert(FSP_SUCCESS == err);
#if G_HS300X_SENSOR0_NON_BLOCKING
        while (!g_hs300x_completed)
        {
            ;
        }
        g_hs300x_completed = false;
#endif

        /* Calculate humidity and temperature values from ADC data */
        err = g_hs300x_sensor0.p_api->dataCalculate (g_hs300x_sensor0.p_ctrl, &hs300x_raw_data, p_data);
        if (FSP_SUCCESS == err)
        {
            is_valid_data = true;
        }
        else if (FSP_ERR_SENSOR_INVALID_DATA == err)
        {
            is_valid_data = false;
        }
        else
        {
            assert(false);
        }
    }
    while (false == is_valid_data);

    /* Wait 4 seconds. See table 4 on the page 6 of the datasheet. */
    R_BSP_SoftwareDelay (4, BSP_DELAY_UNITS_SECONDS);
}

/* TODO: Enable if you want to open OB1203 */
#define G_OB1203_SENSOR0_NON_BLOCKING (1)
#define G_OB1203_SENSOR0_IRQ_ENABLE   (0)

#if G_OB1203_SENSOR0_NON_BLOCKING
volatile bool g_ob1203_i2c_completed = false;
#endif
#if G_OB1203_SENSOR0_IRQ_ENABLE
volatile bool g_ob1203_irq_completed = false;
#endif

/* TODO: Enable if you want to use a callback */
#define G_OB1203_SENSOR0_CALLBACK_ENABLE (1)
#if G_OB1203_SENSOR0_CALLBACK_ENABLE
void ob1203_comms_i2c_callback(rm_ob1203_callback_args_t *p_args)
{
#if G_OB1203_SENSOR0_NON_BLOCKING
    if (RM_OB1203_EVENT_ERROR != p_args->event)
    {
        g_ob1203_i2c_completed = true;
    }
#else
    FSP_PARAMETER_NOT_USED(p_args);
#endif
}
#endif

/* TODO: Enable if you want to use a IRQ callback */
#define G_OB1203_SENSOR0_IRQ_CALLBACK_ENABLE (0)
#if G_OB1203_SENSOR0_IRQ_CALLBACK_ENABLE
void ob1203_irq_callback(rm_ob1203_callback_args_t * p_args)
{
#if ob1203_name_upper_IRQ_ENABLE
    FSP_PARAMETER_NOT_USED(p_args);
    g_ob1203_irq_completed = true;
#else
    FSP_PARAMETER_NOT_USED(p_args);
#endif
}
#endif

/* Quick setup for ob1203_name.
 * - i2c_bus_name must be setup before calling this function
 *     (See Developer Assistance -> ob1203_name -> OB1203 ***** on rm_ob1203 -> i2c_device_name -> i2c_bus_name -> Quick Setup).
 */

/* Quick setup for ob1203_name. */
void g_ob1203_sensor0_quick_setup(void)
{
    fsp_err_t err;

    /* Open OB1203 instance, this must be done before calling any OB1203 API */
    err = g_ob1203_sensor0.p_api->open (g_ob1203_sensor0.p_ctrl, g_ob1203_sensor0.p_cfg);
    assert(FSP_SUCCESS == err);
}

/* Quick getting Light mode values for ob1203_name.
 * - ob1203_name must be setup before calling this function.
 */

/* Quick getting Light data for ob1203_name. */
void g_ob1203_sensor0_quick_getting_light_mode_data(rm_ob1203_light_data_t *p_ob1203_data,
        rm_ob1203_light_data_type_t data_type)
{
    fsp_err_t err;
    rm_ob1203_raw_data_t ob1203_raw_data;
#if 0 == ob1203_name_upper_IRQ_ENABLE
    rm_ob1203_device_status_t device_status;
#endif

    /* Clear I2C callback flag */
#if G_OB1203_SENSOR0_NON_BLOCKING
    g_ob1203_i2c_completed = false;
#endif

    /* Start the measurement */
    /* If the MeasurementStart API is called once, a second call is not required. */
    err = g_ob1203_sensor0.p_api->measurementStart (g_ob1203_sensor0.p_ctrl);
    assert(FSP_SUCCESS == err);
#if G_OB1203_SENSOR0_NON_BLOCKING
    while (!g_ob1203_i2c_completed)
    {
        ;
    }
    g_ob1203_i2c_completed = false;
#endif

    /* If IRQ interrupt is enabled, wait IRQ callback */
#if G_OB1203_SENSOR0_IRQ_ENABLE
    while (!g_ob1203_irq_completed)
    {
        ;
    }
    g_ob1203_irq_completed = false;
#else
    /* Wait until the measurement is complete */
    do
    {
        /* Get device status */
        err = g_ob1203_sensor0.p_api->deviceStatusGet (g_ob1203_sensor0.p_ctrl, &device_status);
        assert(FSP_SUCCESS == err);
#if G_OB1203_SENSOR0_NON_BLOCKING
        while (!g_ob1203_i2c_completed)
        {
            ;
        }
        g_ob1203_i2c_completed = false;
#endif
    }
    while (false == device_status.light_measurement_complete);
#endif

    /* Read ADC data */
    err = g_ob1203_sensor0.p_api->lightRead (g_ob1203_sensor0.p_ctrl, &ob1203_raw_data, data_type);
    assert(FSP_SUCCESS == err);
#if G_OB1203_SENSOR0_NON_BLOCKING
    while (!g_ob1203_i2c_completed)
    {
        ;
    }
    g_ob1203_i2c_completed = false;
#endif

    /* Calculate Light data */
    err = g_ob1203_sensor0.p_api->lightDataCalculate (g_ob1203_sensor0.p_ctrl, &ob1203_raw_data, p_ob1203_data);
    assert(FSP_SUCCESS == err);
}

/* Quick getting Proximity mode values for ob1203_name.
 * - ob1203_name must be setup before calling this function.
 */

/* Quick getting Proximity data for ob1203_name. */
void g_ob1203_sensor0_quick_getting_proximity_mode_data(rm_ob1203_prox_data_t *p_ob1203_data)
{
    fsp_err_t err;
    rm_ob1203_raw_data_t ob1203_raw_data;
#if 0 == G_OB1203_SENSOR0_IRQ_ENABLE
    rm_ob1203_device_status_t device_status;
#endif

    /* Clear I2C callback flag */
#if G_OB1203_SENSOR0_NON_BLOCKING
    g_ob1203_i2c_completed = false;
#endif

    /* Start the measurement */
    /* If the MeasurementStart API is called once, a second call is not required. */
    err = g_ob1203_sensor0.p_api->measurementStart (g_ob1203_sensor0.p_ctrl);
    assert(FSP_SUCCESS == err);
#if G_OB1203_SENSOR0_NON_BLOCKING
    while (!g_ob1203_i2c_completed)
    {
        ;
    }
    g_ob1203_i2c_completed = false;
#endif

    /* If IRQ interrupt is enabled, wait IRQ callback */
#if G_OB1203_SENSOR0_IRQ_ENABLE
    while (!g_ob1203_irq_completed)
    {
        ;
    }
    g_ob1203_irq_completed = false;
#else
    /* Wait until the measurement is complete */
    do
    {
        /* Get device status */
        err = g_ob1203_sensor0.p_api->deviceStatusGet (g_ob1203_sensor0.p_ctrl, &device_status);
        assert(FSP_SUCCESS == err);
#if G_OB1203_SENSOR0_NON_BLOCKING
        while (!g_ob1203_i2c_completed)
        {
            ;
        }
        g_ob1203_i2c_completed = false;
#endif
    }
    while (false == device_status.prox_measurement_complete);
#endif

    /* Read ADC data */
    err = g_ob1203_sensor0.p_api->proxRead (g_ob1203_sensor0.p_ctrl, &ob1203_raw_data);
    assert(FSP_SUCCESS == err);
#if G_OB1203_SENSOR0_NON_BLOCKING
    while (!g_ob1203_i2c_completed)
    {
        ;
    }
    g_ob1203_i2c_completed = false;
#endif

    /* Calculate Proximity data */
    err = g_ob1203_sensor0.p_api->proxDataCalculate (g_ob1203_sensor0.p_ctrl, &ob1203_raw_data, p_ob1203_data);
    assert(FSP_SUCCESS == err);
}
