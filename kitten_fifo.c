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
    /*check if fifo is inited*/
    if(!fifo->is_inited){
        return KITTEN_FIFO_ERROR_FIFO_NOT_INIT;
    }
    /*begin to clear*/
    fifo->irq_disable();
    fifo->head = 0;
    fifo->tail = 0;
    fifo->used_size = 0;
    fifo->is_writing = false;
    fifo->irq_enable();
    return KITTEN_FIFO_NOERROR;
}

kitten_fifo_error_t kitten_fifo_write_with_memcpy(kitten_fifo_t *fifo,uint8_t *data,uint16_t size){
    /*check function arguments*/
    if(fifo == NULL || data == NULL || size == 0){
        return KITTEN_FIFO_ERROR_ARGS;
    }
    /*check if fifo is inited*/
    if(!fifo->is_inited){
        return KITTEN_FIFO_ERROR_FIFO_NOT_INIT;
    }
    /*check if fifo is writing*/
    if(fifo->is_writing){
        return KITTEN_FIFO_ERROR_ISWRITING;
    }else{
        fifo->irq_disable();
        fifo->is_writing = true;
        fifo->irq_enable();
    }
    /*check fifo size*/
    if(size > fifo->fifo_size - fifo->used_size){
        fifo->irq_disable();
        fifo->is_writing = false;
        fifo->irq_enable();
        return KITTEN_FIFO_ERROR_NOSPACE;
    }
    /*begin to memcpy*/
    if(fifo->fifo_size - fifo->head >= size){
        memcpy(fifo->buf + fifo->head, data, size);
    }else{
        memcpy(fifo->buf + fifo->head, data, fifo->fifo_size - fifo->head);
        memcpy(fifo->buf, data + fifo->fifo_size - fifo->head, size - (fifo->fifo_size - fifo->head));
    }
    /*update fifo status*/
    kitten_fifo_write_with_nomemcpycpt(fifo,size);
    return KITTEN_FIFO_NOERROR;
}

kitten_fifo_error_t kitten_fifo_write_with_nomemcpy(kitten_fifo_t *fifo){
    /*check function arguments*/
    if(fifo == NULL){
        return KITTEN_FIFO_ERROR_ARGS;
    }
    /*check if fifo is inited`*/
    if(!fifo->is_inited){
        return KITTEN_FIFO_ERROR_FIFO_NOT_INIT;
    }
    /*check if fifo is writing*/
    if(fifo->is_writing){
        return KITTEN_FIFO_ERROR_ISWRITING;
    }else{
        fifo->irq_disable();
        fifo->is_writing = true;
        fifo->irq_enable();
    }
    return KITTEN_FIFO_NOERROR;
}

kitten_fifo_error_t kitten_fifo_write_with_nomemcpycpt(kitten_fifo_t *fifo,uint16_t size){
    /*check function arguments*/
    if(fifo == NULL || size == 0){
        return KITTEN_FIFO_ERROR_ARGS;
    }
    /*check if fifo is inited*/
    if(!fifo->is_inited){
        return KITTEN_FIFO_ERROR_FIFO_NOT_INIT;
    }
    /*check if fifo is writing*/
    if(!fifo->is_writing){
        return KITTEN_FIFO_ERROR_NOTWRITING;
    }
    /*update fifo status*/
    fifo->irq_disable();
    fifo->head = (fifo->head + size) % fifo->fifo_size;
    fifo->used_size += size;
    fifo->is_writing = false;
    fifo->irq_enable();
    return KITTEN_FIFO_NOERROR;
}