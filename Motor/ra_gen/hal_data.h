/* generated HAL header file - do not edit */
#ifndef HAL_DATA_H_
#define HAL_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "common_data.h"
#include "r_sci_uart.h"
#include "r_uart_api.h"
#include "rm_comms_i2c.h"
#include "rm_comms_api.h"
FSP_HEADER
/** UART on SCI Instance. */
extern const uart_instance_t g_console_write;

/** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
extern sci_uart_instance_ctrl_t g_console_write_ctrl;
extern const uart_cfg_t g_console_write_cfg;
extern const sci_uart_extended_cfg_t g_console_write_cfg_extend;

#ifndef user_uart_callback
void user_uart_callback(uart_callback_args_t *p_args);
#endif
/* I2C Communication Device */
extern const rm_comms_instance_t g_MPU6050;
extern rm_comms_i2c_instance_ctrl_t g_MPU6050_ctrl;
extern const rm_comms_cfg_t g_MPU6050_cfg;
#ifndef MPU6050_callback
void MPU6050_callback(rm_comms_callback_args_t *p_args);
#endif
void hal_entry(void);
void g_hal_init(void);
FSP_FOOTER
#endif /* HAL_DATA_H_ */
