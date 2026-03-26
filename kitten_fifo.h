/**
 * @file kitten_fifo.h
 * @brief A simple FIFO library for embedded systems.
 * @author kitten-yyds
 * @date 2026-03-26
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
    uint8_t *buf;
    uint16_t fifo_size;
    volatile uint16_t head;
    volatile uint16_t tail;
    volatile uint16_t used_size;
    volatile bool is_inited;
    volatile bool is_writing;
    volatile bool is_reading;
    void (*fifo_error_handle)(kitten_fifo_error_t);
    void (*irq_disable)(void);
    void (*irq_enable)(void);
}kitten_fifo_t;

kitten_fifo_error_t kitten_fifo_init(kitten_fifo_config_t *config,kitten_fifo_t *fifo);
kitten_fifo_error_t kitten_fifo_clear(kitten_fifo_t *fifo);
kitten_fifo_error_t kitten_fifo_write_with_memcpy(kitten_fifo_t *fifo,uint8_t *data,uint16_t size);
kitten_fifo_error_t kitten_fifo_write_with_nomemcpy(kitten_fifo_t *fifo);
kitten_fifo_error_t kitten_fifo_write_cpt(kitten_fifo_t *fifo,uint16_t size);
kitten_fifo_error_t kitten_fifo_read_with_memcpy(kitten_fifo_t *fifo,uint8_t *data,uint16_t *size);
kitten_fifo_error_t kitten_fifo_read_with_nomemcpy(kitten_fifo_t *fifo);
kitten_fifo_error_t kitten_fifo_read_cpt(kitten_fifo_t *fifo,uint16_t size);

#endif