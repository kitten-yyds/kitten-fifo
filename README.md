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
First, you need to include the header file `kitten_fifo.h` in your project.  
```
#include "kitten_fifo.h"
```
Then, make sure that your build system (Such as Makefile or Cmake) includes `kitten_fifo.c` in the compilation and that `kitten_fifo.h` is in the include path.

Second, you need to create a FIFO buffer,like this:
```
#define FIFO_SIZE 256
uint8_t fifo_buffer[FIFO_SIZE];
```
or you can use dynamic memory allocation:
```
#define FIFO_SIZE 256
uint8_t *fifo_buffer = (uint8_t *)malloc(FIFO_SIZE);
```