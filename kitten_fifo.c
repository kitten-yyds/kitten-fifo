#include "kitten_fifo.h"

kitten_fifo_error_t kitten_fifo_init(kitten_fifo_config_t *config,kitten_fifo_t *fifo){
    /*check function arguments*/
    if(config == NULL || fifo == NULL){
        return KITTEN_FIFO_ERROR_ARGS;
    }
    /*check fifo config*/
    if(config->buf == NULL || config->fifo_size == 0 ||
        config->irq_disable == NULL || config->irq_enable == NULL){
        return KITTEN_FIFO_ERROR_CONFIG;
    }
    /*begin to config*/
    fifo->buf = config->buf;
    fifo->fifo_size = config->fifo_size;
    fifo->head = 0;
    fifo->tail = 0;
    fifo->used_size = 0;
    fifo->write_size = 0;
    fifo->read_size = 0;
    fifo->is_writing = false;
    fifo->is_inited = true;
    fifo->fifo_error_handle = config->fifo_error_handle;
    fifo->irq_disable = config->irq_disable;
    fifo->irq_enable = config->irq_enable;
    return KITTEN_FIFO_NOERROR;
}

kitten_fifo_error_t kitten_fifo_clear(kitten_fifo_t *fifo){
    /*check function arguments*/
    if(fifo == NULL){
        return KITTEN_FIFO_ERROR_ARGS;
    }
    /*check fifo is inited*/
    if(!fifo->is_inited){
        return KITTEN_FIFO_ERROR_FIFO_NOT_INIT;
    }
    /*begin to clear*/
    fifo->head = 0;
    fifo->tail = 0;
    fifo->used_size = 0;
    fifo->write_size = 0;
    fifo->read_size = 0;
    fifo->is_writing = false;
    return KITTEN_FIFO_NOERROR;
}