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
    KITTEN_FIFO_ERROR_EMPTY,
    KITTEN_FIFO_ERROR_FULL,
    KITTEN_FIFO_ERROR_NOPEEK,
    KITTEN_FIFO_ERROR_NODEQUEUE,
}kitten_fifo_error_t;

typedef struct{
    uint8_t *buf;
    uint16_t fifo_size;
    uint16_t peek_max_size;
    void (*fifo_error_handle)(kitten_fifo_error_t);
}kitten_fifo_config_t;

typedef struct{
    uint8_t *buf;
    uint16_t fifo_size;
    uint16_t head;
    uint16_t tail;
    uint16_t used_size;
    uint16_t peek_size;
    uint16_t peek_max_size;
    void (*fifo_error_handle)(kitten_fifo_error_t);
}kitten_fifo_t;

bool _kitten_fifo_check_fifo_is_init(kitten_fifo_t *fifo);
kitten_fifo_error_t kitten_fifo_init(kitten_fifo_config_t *fifo_config,kitten_fifo_t *fifo);
kitten_fifo_error_t kitten_fifo_clear(kitten_fifo_t *fifo);
kitten_fifo_error_t kitten_fifo_enqueue(kitten_fifo_t *fifo,uint8_t *data,uint16_t size);
kitten_fifo_error_t kitten_fifo_peek(kitten_fifo_t *fifo,uint8_t *data,uint16_t *size);
kitten_fifo_error_t kitten_fifo_dequeue(kitten_fifo_t *fifo);

#endif