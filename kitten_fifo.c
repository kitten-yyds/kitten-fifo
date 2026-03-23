#include "kitten_fifo.h"

bool _kitten_fifo_check_fifo_is_init(kitten_fifo_t *fifo){
    if(fifo == NULL){
        return false;
    }
    if(fifo->buf == NULL || fifo->fifo_size == 0){
        return false;
    }
    return true;
}

kitten_fifo_error_t kitten_fifo_init(kitten_fifo_config_t *fifo_config,kitten_fifo_t *fifo){
    if(fifo_config == NULL){
        return KITTEN_FIFO_ERROR_ARGS;
    }
    if(fifo == NULL){
        if(fifo_config->fifo_error_handle != NULL){
            fifo_config->fifo_error_handle(KITTEN_FIFO_ERROR_ARGS);
        }
        return KITTEN_FIFO_ERROR_ARGS;
    }
    if(fifo_config->buf == NULL || fifo_config->fifo_size == 0){
        if(fifo_config->fifo_error_handle != NULL){
            fifo_config->fifo_error_handle(KITTEN_FIFO_ERROR_CONFIG);
        }
        return KITTEN_FIFO_ERROR_CONFIG;
    }
    if(fifo_config->peek_max_size > fifo_config->fifo_size){
        if(fifo_config->fifo_error_handle != NULL){
            fifo_config->fifo_error_handle(KITTEN_FIFO_ERROR_CONFIG);
        }
        return KITTEN_FIFO_ERROR_CONFIG;
    }
    fifo->buf = fifo_config->buf;
    fifo->fifo_size = fifo_config->fifo_size;
    fifo->peek_max_size = fifo_config->peek_max_size;
    fifo->fifo_error_handle = fifo_config->fifo_error_handle;
    fifo->head = 0;
    fifo->tail = 0;
    fifo->used_size = 0;
    fifo->peek_size = 0;
    return KITTEN_FIFO_NOERROR;
}

kitten_fifo_error_t kitten_fifo_clear(kitten_fifo_t *fifo){
    if(fifo == NULL){
        return KITTEN_FIFO_ERROR_ARGS;
    }
    if(!_kitten_fifo_check_fifo_is_init(fifo)){
        if(fifo->fifo_error_handle != NULL){
            fifo->fifo_error_handle(KITTEN_FIFO_ERROR_FIFO_NOT_INIT);
        }
        return KITTEN_FIFO_ERROR_FIFO_NOT_INIT;
    }
    fifo->head = 0;
    fifo->tail = 0;
    fifo->used_size = 0;
    fifo->peek_size = 0;
    memset(fifo->buf, 0, fifo->fifo_size);
    return KITTEN_FIFO_NOERROR;
}

kitten_fifo_error_t kitten_fifo_enqueue(kitten_fifo_t *fifo,uint8_t *data,uint16_t size){
    if(fifo == NULL || data == NULL || size == 0){
        return KITTEN_FIFO_ERROR_ARGS;
    }
    if(!_kitten_fifo_check_fifo_is_init(fifo)){
        if(fifo->fifo_error_handle != NULL){
            fifo->fifo_error_handle(KITTEN_FIFO_ERROR_FIFO_NOT_INIT);
        }
        return KITTEN_FIFO_ERROR_FIFO_NOT_INIT;
    }
    if(size > fifo->fifo_size - fifo->used_size){//check if the fifo is full
        if(fifo->fifo_error_handle != NULL){
            fifo->fifo_error_handle(KITTEN_FIFO_ERROR_FULL);
        }
        return KITTEN_FIFO_ERROR_FULL;
    }
    if(size <= fifo->fifo_size - fifo->head){
        memcpy(fifo->buf + fifo->head, data, size);
        fifo->head += size;
        if(fifo->head == fifo->fifo_size){
            fifo->head = 0;
        }
        fifo->used_size += size;
    }else{
        memcpy(fifo->buf + fifo->head, data, fifo->fifo_size - fifo->head);
        memcpy(fifo->buf, data + fifo->fifo_size - fifo->head, size - (fifo->fifo_size - fifo->head));
        fifo->head = size - (fifo->fifo_size - fifo->head);
        fifo->used_size += size;
    }
    return KITTEN_FIFO_NOERROR;
}

kitten_fifo_error_t kitten_fifo_peek(kitten_fifo_t *fifo,uint8_t *data,uint16_t *size){
    if(fifo == NULL || data == NULL || size == NULL){
        return KITTEN_FIFO_ERROR_ARGS;
    }
    if(!_kitten_fifo_check_fifo_is_init(fifo)){
        if(fifo->fifo_error_handle != NULL){
            fifo->fifo_error_handle(KITTEN_FIFO_ERROR_FIFO_NOT_INIT);
        }
        return KITTEN_FIFO_ERROR_FIFO_NOT_INIT;
    }
    if(fifo->used_size == 0){
        if(fifo->fifo_error_handle != NULL){
            fifo->fifo_error_handle(KITTEN_FIFO_ERROR_EMPTY);
        }
        return KITTEN_FIFO_ERROR_EMPTY;
    }
    if(fifo->peek_size != 0){
        if(fifo->fifo_error_handle != NULL){
            fifo->fifo_error_handle(KITTEN_FIFO_ERROR_NODEQUEUE);
        }
        return KITTEN_FIFO_ERROR_NODEQUEUE;
    }
    if(fifo->tail <= fifo->head){
        if(fifo->peek_max_size == 0){
            memcpy(data,fifo->buf + fifo->tail, fifo->used_size);
            fifo->peek_size = fifo->used_size;
        }else{
            if(fifo->used_size > fifo->peek_max_size){
                memcpy(data,fifo->buf + fifo->tail, fifo->peek_max_size);
                fifo->peek_size = fifo->peek_max_size;
            }else{
                memcpy(data,fifo->buf + fifo->tail, fifo->used_size);
                fifo->peek_size = fifo->used_size;
            }
        }
    }else{
        if(fifo->peek_max_size == 0){
            memcpy(data,fifo->buf + fifo->tail, fifo->fifo_size - fifo->tail);
            fifo->peek_size = fifo->fifo_size - fifo->tail;
        }else{
            if(fifo->fifo_size - fifo->tail > fifo->peek_max_size){
                memcpy(data,fifo->buf + fifo->tail, fifo->peek_max_size);
                fifo->peek_size = fifo->peek_max_size;
            }else{
                memcpy(data,fifo->buf + fifo->tail, fifo->fifo_size - fifo->tail);
                fifo->peek_size = fifo->fifo_size - fifo->tail;
            }
        }
    }
    *size = fifo->peek_size;
    return KITTEN_FIFO_NOERROR;
}

kitten_fifo_error_t kitten_fifo_dequeue(kitten_fifo_t *fifo){
    if(fifo == NULL){
        return KITTEN_FIFO_ERROR_ARGS;
    }
    if(!_kitten_fifo_check_fifo_is_init(fifo)){
        if(fifo->fifo_error_handle != NULL){
            fifo->fifo_error_handle(KITTEN_FIFO_ERROR_FIFO_NOT_INIT);
        }
        return KITTEN_FIFO_ERROR_FIFO_NOT_INIT;
    }
    if(fifo->used_size == 0){
        if(fifo->fifo_error_handle != NULL){
            fifo->fifo_error_handle(KITTEN_FIFO_ERROR_EMPTY);
        }
        return KITTEN_FIFO_ERROR_EMPTY;
    }
    if(fifo->peek_size == 0){
        if(fifo->fifo_error_handle != NULL){
            fifo->fifo_error_handle(KITTEN_FIFO_ERROR_NOPEEK);
        }
        return KITTEN_FIFO_ERROR_NOPEEK;
    }
    fifo->tail += fifo->peek_size;
    if(fifo->tail >= fifo->fifo_size){
        fifo->tail -= fifo->fifo_size;
    }
    fifo->used_size -= fifo->peek_size;
    fifo->peek_size = 0;
    return KITTEN_FIFO_NOERROR;
}