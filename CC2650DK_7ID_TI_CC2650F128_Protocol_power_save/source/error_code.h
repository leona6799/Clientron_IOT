/*
 * error_code.h
 *
 *  Created on: 2016¦~7¤ë27¤é
 *      Author: 03565
 */

#ifndef SOURCE_ERROR_CODE_H_
#define SOURCE_ERROR_CODE_H_

#define EXECUTE_SUCCESS                 0x00000000
#define EXECUTE_FAIL                    0x80000000

// Common error
#define PARAMETER_INVALID               (EXECUTE_FAIL | 0x0000)
#define INIT_LOG_ERROR                  (EXECUTE_FAIL | 0x0001)
//#define GET_CONFIG_ERROR                (EXECUTE_FAIL | 0x0002)
//#define STORE_CONFIG_ERROR              (EXECUTE_FAIL | 0x0003)
#define PAYLOAD_LEN_INVALID             (EXECUTE_FAIL | 0x0004)
#define PAYLOAD_PARAMETER_INVALID       (EXECUTE_FAIL | 0x0005)
#define SYSTEM_NOT_START                (EXECUTE_FAIL | 0x0006)
#define SYSTEM_ALREADY_START            (EXECUTE_FAIL | 0x0007)



// Catch_value
#define SENSOR_ID_NOT_FOUND             (EXECUTE_FAIL | (0x0001 << 16))
#define SENSOR_BUS_NOT_FOUND            (EXECUTE_FAIL | (0x0002 << 16))
#define BUS_OPEN_FAIL                   (EXECUTE_FAIL | (0x0003 << 16))
#define SENSOR_CONFIG_FAIL              (EXECUTE_FAIL | (0x0004 << 16))
#define GET_VALUE_FAIL                  (EXECUTE_FAIL | (0x0005 << 16))

// Dosconnect
#define DISCONNECT_FAIL                 (EXECUTE_FAIL | (0x0001 << 16))

// Get log
#define GET_LOG_FIRST_ERROR             (EXECUTE_FAIL | (0x0001 << 16))
#define GET_LOG_NEXT_ERROR              (EXECUTE_FAIL | (0x0002 << 16))

// Clear log
#define CLEAR_LOG_CLEAR_ERROR           (EXECUTE_FAIL | (0x0001 << 16))

// Set LoRa config
#define SET_LORA_GET_CONFIG_ERROR       (EXECUTE_FAIL | (0x0001 << 16))
#define SET_LORA_STORE_CONFIG_ERROR     (EXECUTE_FAIL | (0x0002 << 16))

// Get LoRa config

// Receive ACK

// Set sensor setting
#define SET_SENSOR_GET_CONFIG_ERROR     (EXECUTE_FAIL | (0x0001 << 16))
#define SET_SENSOR_STORE_CONFIG_ERROR   (EXECUTE_FAIL | (0x0002 << 16))

// Get seneor setting

// System info

// System start
#define START_INIT_LOG_ERROR            (EXECUTE_FAIL | (0x0001 << 16))
#define START_GET_CONFIG_ERROR          (EXECUTE_FAIL | (0x0002 << 16))
#define START_STORE_CONFIG_ERROR        (EXECUTE_FAIL | (0x0003 << 16))

// Time sync


// Store log
#define FLASH_PARAMETER_INVALID         (EXECUTE_FAIL | (0x0001 << 8))
#define FLASH_LOG_EMPTY                 (EXECUTE_FAIL | (0x0002 << 8))
#define FLASH_LOG_LAST                  (EXECUTE_FAIL | (0x0003 << 8))
#define FLASH_OPEN_ERROR                (EXECUTE_FAIL | (0x0004 << 8))
#define FLASH_READ_ERROR                (EXECUTE_FAIL | (0x0005 << 8))
#define FLASH_WRITE_ERROR               (EXECUTE_FAIL | (0x0006 << 8))
#define FLASH_ERASE_ERROR               (EXECUTE_FAIL | (0x0006 << 8))

// Clear log

#endif /* SOURCE_ERROR_CODE_H_ */
