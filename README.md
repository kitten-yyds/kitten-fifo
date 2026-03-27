# A Simple FIFO library for embedded systems
This is a simple FIFO library for embedded systems. It provides a simple API for creating and managing FIFO buffers. The library is designed to be lightweight and efficient, making it suitable for use in embedded systems with limited resources.    

## Features
- Simple API for creating and managing FIFO buffers
- Lightweight and efficient
- Suitable for use in embedded systems with limited resources
- Supports both static and dynamic memory allocation

## State
Has been tested on STM32H750XBH6 microcontroller with noRTOS. It is currently in the early stages of development and may contain bugs. Use at your own risk.

## Usage
To use the library, you need to follow these steps:     
**Include Library** -> **Init FIFO Buffer** -> **Write/Read FIFO**
### Include Library
You need to include the header file `kitten_fifo.h` in your project.  
```
#include "kitten_fifo.h"
```
> Ensure that your build system (Such as Makefile or CMake) adds `kitten_fifo.c` to the compilation sources, and that `kitten_fifo.h` is in your include path.

### Init FIFO Buffer
You need to create a FIFO buffer, like this: 
```
#define FIFO_SIZE 256
uint8_t fifo_buffer[FIFO_SIZE];
```
Or you can use dynamic memory allocation:
```
#define FIFO_SIZE 256
uint8_t *fifo_buffer = (uint8_t *)malloc(FIFO_SIZE);
```
After that, you can initialize the FIFO buffer using the `kitten_fifo_init` function:
```
kitten_fifo_config_t fifo_config = {
    .buf = fifo_buffer,
    .fifo_size = FIFO_SIZE,
    .fifo_error_handle = NULL,
    .irq_disable = __disable_irq,
    .irq_enable = __enable_irq,
};
kitten_fifo_t fifo;
kitten_fifo_init(&fifo_config, &fifo);
```