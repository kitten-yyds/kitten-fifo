#ifndef KITTEN_FIFO_H
#define KITTEN_FIFO_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

typedef enum{
    KITTEN_FIFO_NOERROR = 0,
    KITTEN_FIFO_ERROR_ARGS,//function args lost
    KITTEN_FIFO_ERROR_CONFIG,//config value error
    KITTEN_FIFO_ERROR_FIFO_NOT_INIT,//fifo not init
    KITTEN_FIFO_ERROR_ISWRITING,//fifo is writing
    KITTEN_FIFO_ERROR_NOTWRITTEN,//fifo has not been written
    KITTEN_FIFO_ERROR_ISREADING,//fifo is reading
    KITTEN_FIFO_ERROR_NOTREAD,//fifo has not been read
    KITTEN_FIFO_ERROR_NOSPACE,//fifo is no space to write
    KITTEN_FIFO_ERROR_NODATA,//fifo is no data to read
}kitten_fifo_error_t;

typedef struct{
    uint8_t *buf;
    uint16_t fifo_size;
    void (*fifo_error_handle)(kitten_fifo_error_t);
    void (*irq_disable)(void);
    void (*irq_enable)(void);
}kitten_fifo_config_t;

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