/**
 * @file kitten_fifo.h
 * @brief A simple FIFO library for embedded systems.
 * @author kitten-yyds
 * @date 2026-03-27
 * @version 1.0
 */

#ifndef KITTEN_FIFO_H
#define KITTEN_FIFO_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>


/**
 * @enum kitten_fifo_error_t
 * @brief Error codes returned by kitten FIFO functions.
 */
typedef enum {
    KITTEN_FIFO_NOERROR = 0,              /**< No error */
    KITTEN_FIFO_ERROR_ARGS,               /**< Missing or invalid function arguments */
    KITTEN_FIFO_ERROR_CONFIG,             /**< Invalid configuration */
    KITTEN_FIFO_ERROR_FIFO_NOT_INIT,      /**< FIFO not initialized */
    KITTEN_FIFO_ERROR_ISWRITING,          /**< Cannot write: FIFO is currently being written */
    KITTEN_FIFO_ERROR_NOTWRITTEN,         /**< No previous write; call kitten_fifo_write_with_nomemcpy first */
    KITTEN_FIFO_ERROR_ISREADING,          /**< Cannot read: FIFO is currently being read */
    KITTEN_FIFO_ERROR_NOTREAD,            /**< No previous read; call kitten_fifo_read_with_nomemcpy first */
    KITTEN_FIFO_ERROR_NOSPACE,            /**< Not enough space to write; increase fifo_size or read data */
    KITTEN_FIFO_ERROR_NODATA,             /**< No data available to read */
} kitten_fifo_error_t;

/**
 * @struct kitten_fifo_config_t
 * @brief Configuration structure for initializing a kitten FIFO.
 */
typedef struct{
    uint8_t *buf;                                   /**< Pointer to the FIFO buffer. */
    uint16_t fifo_size;                             /**< Size of the FIFO buffer (in bytes). */
    void (*fifo_error_handle)(kitten_fifo_error_t); /**< Error callback (can be NULL). Called on error; keep handler short. [Reserved] */
    void (*irq_disable)(void);                      /**< Interrupt disable callback (must NOT be NULL). */
    void (*irq_enable)(void);                       /**< Interrupt enable callback (must NOT be NULL). */
}kitten_fifo_config_t;

/**
 * @struct kitten_fifo_t
 * @brief Structure representing a kitten FIFO.
 */
typedef struct{
    uint8_t *buf;                                   /**< Pointer to the FIFO buffer. */
    uint16_t fifo_size;                             /**< Size of the FIFO buffer (in bytes). */
    volatile uint16_t head;                         /**< Head index of the FIFO. */
    volatile uint16_t tail;                         /**< Tail index of the FIFO. */
    volatile uint16_t used_size;                    /**< Number of bytes currently in the FIFO. */
    volatile bool is_inited;                        /**< Indicates whether the FIFO has been initialized. */
    volatile bool is_writing;                       /**< Indicates whether a write operation is in progress. */
    volatile bool is_reading;                       /**< Indicates whether a read operation is in progress. */
    void (*fifo_error_handle)(kitten_fifo_error_t); /**< Error callback (can be NULL). Called on error; keep handler short. [Reserved] */
    void (*irq_disable)(void);                      /**< Interrupt disable callback (must NOT be NULL). */
    void (*irq_enable)(void);                       /**< Interrupt enable callback (must NOT be NULL). */
}kitten_fifo_t;

/* Function */
/**
 * @brief Initializes a kitten FIFO with the specified configuration.
 * @param config Pointer to the FIFO configuration structure.
 * @param fifo Pointer to the FIFO structure to initialize.
 * @return Error code indicating the result of the initialization.
 */
kitten_fifo_error_t kitten_fifo_init(kitten_fifo_config_t *config,kitten_fifo_t *fifo);

/**
 * @brief Clears the contents of the FIFO, resetting head, tail, and used size.
 * @param fifo Pointer to the FIFO structure to clear.
 * @return Error code indicating the result of the clear operation.
 */
kitten_fifo_error_t kitten_fifo_clear(kitten_fifo_t *fifo);

/**
 * @brief Writes data to the FIFO using memcpy.
 * @param fifo Pointer to the FIFO structure.
 * @param data Pointer to the data to write.
 * @param size Size of the data to write.
 * @return Error code indicating the result of the write operation.
 */
kitten_fifo_error_t kitten_fifo_write_with_memcpy(kitten_fifo_t *fifo,uint8_t *data,uint16_t size);

/**
 * @brief Begins a write operation without copying data. Caller must call kitten_fifo_write_cpt after writing data to update FIFO status.
 * @param fifo Pointer to the FIFO structure.
 * @return Error code indicating the result of the write operation.
 */
kitten_fifo_error_t kitten_fifo_write_with_nomemcpy(kitten_fifo_t *fifo);

/**
 * @brief Updates FIFO status after a write operation initiated by kitten_fifo_write_with_nomemcpy.
 * @param fifo Pointer to the FIFO structure.
 * @param size Size of the data written to the FIFO.
 * @return Error code indicating the result of the operation.
 */
kitten_fifo_error_t kitten_fifo_write_cpt(kitten_fifo_t *fifo,uint16_t size);

/**
 * @brief Reads data from the FIFO using memcpy.
 * @param fifo Pointer to the FIFO structure.
 * @param data Pointer to the buffer to store read data.
 * @param size Pointer to the size of the data to read. On return, contains the actual size of data read.
 * @return Error code indicating the result of the read operation.
 */
kitten_fifo_error_t kitten_fifo_read_with_memcpy(kitten_fifo_t *fifo,uint8_t *data,uint16_t *size);

/**
 * @brief Begins a read operation without copying data. Caller must call kitten_fifo_read_cpt after reading data to update FIFO status.
 * @param fifo Pointer to the FIFO structure.
 * @return Error code indicating the result of the read operation.
 */
kitten_fifo_error_t kitten_fifo_read_with_nomemcpy(kitten_fifo_t *fifo);

/**
 * @brief Updates FIFO status after a read operation initiated by kitten_fifo_read_with_nomemcpy.
 * @param fifo Pointer to the FIFO structure.
 * @param size Size of the data read from the FIFO.
 * @return Error code indicating the result of the operation.
 */
kitten_fifo_error_t kitten_fifo_read_cpt(kitten_fifo_t *fifo,uint16_t size);

#endif