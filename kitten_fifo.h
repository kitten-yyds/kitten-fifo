#ifndef KITTEN_FIFO_H
#define KITTEN_FIFO_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define KITTEN_FIFO_NOERROR_INFO "No error"
#define KITTEN_FIFO_ERROR_ARGS_INFO "Function arguments error"
#define KITTEN_FIFO_ERROR_CONFIG_INFO "Configuration error"
#define KITTEN_FIFO_ERROR_FIFO_NOT_INIT_INFO "FIFO not initialized. You must do kitten_fifo_init() before using the FIFO"
#define KITTEN_FIFO_ERROR_EMPTY_INFO "FIFO is empty"
#define KITTEN_FIFO_ERROR_FULL_INFO "FIFO is full, please increase the FIFO size or clear the FIFO"
#define KITTEN_FIFO_ERROR_NOPEEK_INFO "No data to dequeue, maybe you have not peek yet or you have peeked all the data but not dequeue them, please peek the data or clear the FIFO"
#define KITTEN_FIFO_ERROR_NODEQUEUE_INFO "Failed to peek data, maybe you have peeked the data but not dequeue them, please dequeue the data or clear the FIFO"

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
    void (*fifo_error_handle)(kitten_fifo_error_t);
    void (*irq_disable)(void);
    void (*irq_enable)(void);
}kitten_fifo_config_t;

typedef struct{
    uint8_t *buf;
    uint16_t fifo_size;
    uint16_t head;
    uint16_t tail;
    uint16_t write_size;
    uint16_t read_size;
    uint16_t used_size;
    bool is_writing;
    void (*fifo_error_handle)(kitten_fifo_error_t);
    void (*irq_disable)(void);
    void (*irq_enable)(void);
}kitten_fifo_t;

kitten_fifo_error_t kitten_fifo_init(kitten_fifo_config_t *config,kitten_fifo_t *fifo);
kitten_fifo_error_t kitten_fifo_clear(kitten_fifo_t *fifo);
kitten_fifo_error_t kitten_fifo_write_with_memcpy(kitten_fifo_t *fifo,uint8_t *data,uint16_t size);
kitten_fifo_error_t kitten_fifo_write_with_nomemcpy(kitten_fifo_t *fifo);
kitten_fifo_error_t kitten_fifo_write_with_nomemcpycpt(kitten_fifo_t *fifo,uint16_t size);
kitten_fifo_error_t kitten_fifo_read_with_memcpy(kitten_fifo_t *fifo,uint8_t *data,uint16_t size);
kitten_fifo_error_t kitten_fifo_read_with_nomemcpy(kitten_fifo_t *fifo);
kitten_fifo_error_t kitten_fifo_read_with_nomemcpycpt(kitten_fifo_t *fifo,uint16_t size);

#endif