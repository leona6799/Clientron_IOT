#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>

#include "source/sensor_hub.h"

#include "ext_flash.h"
#include "store_log.h"

#define STORE_INFO_MARK         0x55AA55AA
#define STORE_INFO_SECTOR       0x00
#define RESERVE_INFO_SPACE      256
#define START_SECTOR_NUM        0x01
#define LAST_SECTOR_NUM         32
#define BUFFER_HAS_WRAPPED      1
#define BUFFER_NOT_WRAPPED      0

#define SIGNATURE_OFFSET    (STORE_INFO_SECTOR * BLS_SECTOR_SIZE)

#define START_INFO_OFFSET   (SIGNATURE_OFFSET + RESERVE_INFO_SPACE)
#define START_RING_BUFFER   (START_SECTOR_NUM * BLS_SECTOR_SIZE)
#define END_RING_BUFFER     (LAST_SECTOR_NUM * BLS_SECTOR_SIZE)

struct Store_Info {
    uint32_t sign;
    uint32_t overlay;
    uint32_t log_count;

    size_t  tail;
    size_t  head;     // Next available space
};

static uint8_t sector_buf[BLS_SECTOR_SIZE];

static void get_default_info (struct Store_Info *info)
{
    info->sign      = STORE_INFO_MARK;
    info->overlay   = BUFFER_NOT_WRAPPED;
    info->log_count = 0;
    info->tail      = START_RING_BUFFER;
    info->head      = START_RING_BUFFER;
}

static struct Store_Info *slide_info_last(uint8_t *start, size_t length)
{
    uint8_t           *end;
    struct Store_Info *iter;

    if (start == NULL)
        return NULL;

    iter = (struct Store_Info *)start;
    end  = start + length;

    if (iter->sign == STORE_INFO_MARK) {
        while (iter->sign == STORE_INFO_MARK && (size_t)iter < (size_t)end)
            ++iter;

        --iter;
    } else {
        return NULL;
    }

    return iter;
}

static int get_store_info(struct Store_Info *info)
{
    struct Store_Info *iter;

    if (info == NULL)
        return FLASH_PARAMETER_INVALID;

    memset(sector_buf, 0x00, sizeof(sector_buf));
    if (extFlashRead(SIGNATURE_OFFSET, sizeof(sector_buf), sector_buf)) {
    	iter = slide_info_last(sector_buf + RESERVE_INFO_SPACE, sizeof(sector_buf));
        if (iter != NULL)
            memcpy(info, iter, sizeof(struct Store_Info));
        else
            get_default_info(info);
    } else {
        return FLASH_READ_ERROR;
    }

    return EXECUTE_SUCCESS;
}

static int write_store_info(struct Store_Info *info)
{
    size_t             offset;
    struct Store_Info *iter;

    if (info == NULL)
        return FLASH_PARAMETER_INVALID;

    offset = SIGNATURE_OFFSET;

    if (extFlashRead(offset, sizeof(sector_buf), sector_buf)) {
        iter = slide_info_last(sector_buf + RESERVE_INFO_SPACE, sizeof(sector_buf));
        if (iter != NULL)
            ++iter;

        if (iter == NULL || (size_t)iter >= (size_t)sector_buf + BLS_SECTOR_SIZE - sizeof(struct Store_Info)) {
            if (sector_buf[RESERVE_INFO_SPACE] != 0xFF) {
                extFlashErase(offset, BLS_SECTOR_SIZE);
                extFlashWrite(offset, RESERVE_INFO_SPACE, (const uint8_t *)sector_buf);
            }

            iter = (struct Store_Info *)&sector_buf[RESERVE_INFO_SPACE];
        }

        offset += (size_t)iter - (size_t)sector_buf;
        if (!extFlashWrite(offset, sizeof(struct Store_Info), (const uint8_t *)info))
            return FLASH_WRITE_ERROR;
    } else {
        return FLASH_READ_ERROR;
    }

    return EXECUTE_SUCCESS;
}

static int modify_sector(size_t offset, const uint8_t *data, int length)
{
    size_t  start_offset;
    size_t  sector_offset;

    if (data == NULL)
    	return FLASH_PARAMETER_INVALID;

    start_offset  = (offset / BLS_SECTOR_SIZE) * BLS_SECTOR_SIZE;
    sector_offset = offset - ((offset / BLS_SECTOR_SIZE) * BLS_SECTOR_SIZE);

    while (length > 0) {
        if (extFlashRead(start_offset, BLS_SECTOR_SIZE, sector_buf))
            memcpy(sector_buf + sector_offset, data, length);
        else
            return FLASH_READ_ERROR;

        if (!extFlashErase(start_offset, BLS_SECTOR_SIZE))
            return FLASH_ERASE_ERROR;

        if (!extFlashWrite(start_offset, sizeof(sector_buf), (const uint8_t *)sector_buf))
            return FLASH_WRITE_ERROR;

        data += BLS_SECTOR_SIZE - sector_offset;
        length -= BLS_SECTOR_SIZE - sector_offset;
        start_offset += BLS_SECTOR_SIZE;
        sector_offset = 0;
    }

    return EXECUTE_SUCCESS;
}

int store_log(const uint8_t *log, size_t size)
{
    int      result;
    uint32_t pre_size;
    uint8_t  buf[EACH_LOG_LENGTH];
    struct log_entry  *entry;
    struct Store_Info  info;


    if (log == NULL || size > (EACH_LOG_LENGTH - sizeof(struct log_entry)))
        return FLASH_PARAMETER_INVALID;


    if (!extFlashOpen())
        return FLASH_OPEN_ERROR;

    result = get_store_info(&info);
    if (result != EXECUTE_SUCCESS)
        goto Exit;

    entry = (struct log_entry *)buf;

    pre_size = System_sprintf(entry->log, "%08x=>", info.log_count++);
    memcpy(entry->log + pre_size, log, size);

    entry->size = size + pre_size;
    size = entry->size + sizeof(struct log_entry);

    if (info.overlay == BUFFER_HAS_WRAPPED) {
        info.tail += EACH_LOG_LENGTH;
        if (info.tail > END_RING_BUFFER - EACH_LOG_LENGTH)
            info.tail = START_RING_BUFFER;

        result = modify_sector(info.head, buf, size);
        if (result != EXECUTE_SUCCESS)
        	goto Exit;
    } else {
        if (!extFlashWrite(info.head, size, (const uint8_t *)buf)) {
            result = FLASH_WRITE_ERROR;
            goto Exit;
        }
    }

    info.head += EACH_LOG_LENGTH;
    if (info.head > END_RING_BUFFER - EACH_LOG_LENGTH) {
        info.overlay  = BUFFER_HAS_WRAPPED;
        info.head = START_RING_BUFFER;
        //info.tail += EACH_LOG_LENGTH;
    }

    result = write_store_info(&info);

Exit:
    extFlashClose();

    return result;
}

int get_first_log(uint8_t *log, uint8_t *size, uint32_t *key)
{
    int    result;
    struct log_entry  *entry;
    struct Store_Info  info;

    if (log == NULL || *size < EACH_LOG_LENGTH || key == NULL)
        return FLASH_PARAMETER_INVALID;

    if (!extFlashOpen())
        return FLASH_OPEN_ERROR;

    result = get_store_info(&info);
    if (result != EXECUTE_SUCCESS)
        goto Exit;

    if (info.overlay == BUFFER_NOT_WRAPPED && info.tail == info.head) {
        result = FLASH_LOG_EMPTY;
        goto Exit;
    }

    if (!extFlashRead(info.tail, EACH_LOG_LENGTH, log)) {
        result = FLASH_READ_ERROR;
        goto Exit;
    }

    entry = (struct log_entry *)log;
    *size = entry->size;
    memcpy(log, entry->log, entry->size);

    result = EXECUTE_SUCCESS;
    *key   = (int)info.tail + EACH_LOG_LENGTH;

    if (*key >= info.head)
        result = FLASH_LOG_LAST;
Exit:
    extFlashClose();
    return result;
}

int get_next_log(uint32_t *key, uint8_t *log, uint8_t *size)
{
    int     result;
    struct log_entry  *entry;
    struct Store_Info info;

    if (key == NULL || log == NULL || *size < EACH_LOG_LENGTH)
        return FLASH_PARAMETER_INVALID;

    if (!extFlashOpen())
        return FLASH_OPEN_ERROR;

    result = get_store_info(&info);
    if (result != EXECUTE_SUCCESS)
        goto Exit;
    else if (*key >= info.head)
        return FLASH_PARAMETER_INVALID;

    if (info.overlay == BUFFER_HAS_WRAPPED && *key > (END_RING_BUFFER - EACH_LOG_LENGTH))
        *key = START_RING_BUFFER;

    if (!extFlashRead(*key, EACH_LOG_LENGTH, log)) {
        result = FLASH_READ_ERROR;
        goto Exit;
    }

    entry = (struct log_entry *)log;
    *size = entry->size;
    memcpy(log, entry->log, *size);

    result = EXECUTE_SUCCESS;
    *key  += EACH_LOG_LENGTH;

    if (*key >= info.head)
        result = FLASH_LOG_LAST;

Exit:
    extFlashClose();
    return result;
}

int spi_clear_log(void)
{
    struct Store_Info info;

    if (!extFlashOpen())
        return FLASH_OPEN_ERROR;

    if (! extFlashErase(START_RING_BUFFER, END_RING_BUFFER - START_RING_BUFFER)) {
        extFlashClose();
        return FLASH_ERASE_ERROR;
    }

    get_store_info(&info);
    info.head    = info.tail;
    info.overlay = BUFFER_NOT_WRAPPED;
    write_store_info(&info);

    extFlashClose();

    return EXECUTE_SUCCESS;
}

int init_log_storage(int cond)
{
    int               result;
    struct config     config;
    struct Store_Info info;

    if (!extFlashOpen())
        return FLASH_OPEN_ERROR;

    if (! extFlashRead(SIGNATURE_OFFSET, sizeof(struct config), (uint8_t *)&config)) {
    	result = FLASH_READ_ERROR;
    	goto Exit;
    }

    result = EXECUTE_SUCCESS;
    if (cond == INIT_ANYWAY || config.first_sign != NOT_FIRST_USE_SIGN) {
        if (cond == INIT_ANYWAY) {
            get_store_info(&info);
            info.head    = info.tail;
            info.overlay = BUFFER_NOT_WRAPPED;
        } else {
            get_default_info(&info);
        }

        if (! extFlashErase(START_RING_BUFFER, END_RING_BUFFER - START_RING_BUFFER)) {
        	result = FLASH_ERASE_ERROR;
            goto Exit;
        }

        if (! extFlashErase(SIGNATURE_OFFSET, BLS_SECTOR_SIZE)) {
        	result = FLASH_ERASE_ERROR;
            goto Exit;
        }

        config.first_sign = NOT_FIRST_USE_SIGN;
        if (! extFlashWrite(SIGNATURE_OFFSET, sizeof(struct config), (uint8_t *)&config)) {
        	result = FLASH_WRITE_ERROR;
            goto Exit;
        }

        write_store_info(&info);
    }

Exit:
    extFlashClose();

    return result;
}

int spi_clear_everything(void)
{
    if (!extFlashOpen())
        return FLASH_OPEN_ERROR;

    if (! extFlashErase(0, BLS_MAX_SECTOR_NUMBER * BLS_SECTOR_SIZE)) {
        extFlashClose();
        return FLASH_ERASE_ERROR;
    }

    extFlashClose();

    return EXECUTE_SUCCESS;
}

int spi_get_config(struct config *config)
{
    if (!extFlashOpen())
        return FLASH_OPEN_ERROR;

    if (!extFlashRead(SIGNATURE_OFFSET, sizeof(struct config), (uint8_t *)config)) {
        extFlashClose();
        return FLASH_READ_ERROR;
    }

    extFlashClose();

    return EXECUTE_SUCCESS;
}

int spi_store_config(struct config *config)
{
	int result;

    if (!extFlashOpen())
        return FLASH_OPEN_ERROR;

    result = modify_sector(SIGNATURE_OFFSET, (const uint8_t *)config, (int)sizeof(struct config));

    extFlashClose();

    return result;
}

