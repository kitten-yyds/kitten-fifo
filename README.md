[简体中文](./README_zh.md)
# A Simple FIFO library for embedded systems
This is a simple FIFO (First In First Out) library for embedded systems. It provides a simple API for creating and managing FIFO buffers. The library is designed to be lightweight and efficient, making it suitable for use in embedded systems with limited resources.    

## Features
- Simple API for creating and managing FIFO buffers
- Lightweight and efficient
- Suitable for use in embedded systems with limited resources
- Supports both static and dynamic memory allocation

## Status
It Has been tested on STM32H750XBH6 microcontroller with noRTOS. It is currently in the early stages of development and may contain bugs. Use at your own risk.

## Usage
To use the library, you need to follow these steps:     
**Include Library** -> **Init FIFO Buffer** -> **Write/Read FIFO**
### Include Library
You need to include the header file `kitten_fifo.h` in your project.  
```
#include "kitten_fifo.h"
```
> Ensure that your build system (such as Makefile or CMake) adds `kitten_fifo.c` to the compilation sources, and that `kitten_fifo.h` is in your include path.

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

> Note:
> - `fifo_size` is a `uint16_t` (maximum 65535); choose an appropriate size.
> - `fifo_error_handle` is an error callback pointer and is currently reserved/unused.
> - The library calls the provided `irq_disable`/`irq_enable` functions to protect critical sections and avoid data tearing.

### Write FIFO
There are two kinds of ways to write data to the FIFO buffer:   
1. Normal write (safe & simple): `kitten_fifo_write_with_memcpy`
2. Zero-copy write (advanced & fast): `kitten_fifo_write_with_nomemcpy` + `kitten_fifo_write_cpt`
#### Normal Write
Function `kitten_fifo_write_with_memcpy` is simple to use. It copies data from the source buffer to the FIFO buffer using `memcpy`. You need to provide three parameters:
1. `fifo`: Pointer to the FIFO you want to operate on.
2. `data`: Pointer to the source buffer.
3. `size`: The size of the data (Number of bytes to write.). 

> It returns `KITTEN_FIFO_NOERROR` if the write operation is successful, otherwise it returns an error code.

Example:
```
kitten_fifo_error_t err = kitten_fifo_write_with_memcpy(&fifo, data, size);
// On success, err == KITTEN_FIFO_NOERROR
```

#### Zero-copy Write
`kitten_fifo_write_with_nomemcpy` acquires the write lock (`is_writing`) but does not copy data or update `head`/`used_size`. You must write directly into the FIFO buffer, then call `kitten_fifo_write_cpt` to commit the written bytes.

Notes:
> - `kitten_fifo_write_cpt` checks free space on commit and will return `KITTEN_FIFO_ERROR_NOSPACE` if there is not enough room.
> - `kitten_fifo_write_cpt` requires `size > 0` and `size <= available_space`; if `size == 0` it returns `KITTEN_FIFO_ERROR_ARGS`.
> - To avoid long-held locks, complete the write and call `kitten_fifo_write_cpt` promptly after `kitten_fifo_write_with_nomemcpy` succeeds.

Example:
```
kitten_fifo_error_t err;
err = kitten_fifo_write_with_nomemcpy(&fifo);
// On success, err == KITTEN_FIFO_NOERROR
// Write data
err = kitten_fifo_write_cpt(&fifo, size);
// On success, err == KITTEN_FIFO_NOERROR
```
### Read FIFO
There are two kinds of ways to read data from the FIFO buffer:
1. Normal read (safe & simple): `kitten_fifo_read_with_memcpy`
2. Zero-copy read (advanced & fast): `kitten_fifo_read_with_nomemcpy` + `kitten_fifo_read_cpt`
#### Normal Read
Function `kitten_fifo_read_with_memcpy` is simple to use. It copies data from the FIFO buffer to the destination buffer using `memcpy`. You need to provide three parameters:
1. `fifo`: Pointer to the FIFO you want to operate on.
2. `data`: Pointer to the destination buffer.
3. `size`: Pointer to a `uint16_t` used as an in/out parameter. On call, if `*size == 0` the function will read all available bytes; otherwise it requests up to `*size` bytes. On return, `*size` is set to the actual number of bytes copied.
> Notes:
> - If the FIFO is empty, the function returns `KITTEN_FIFO_ERROR_NODATA` and sets `*size` to `0`.
> - If the requested `*size` is greater than the number of available bytes, the function copies only the available bytes and updates `*size` accordingly (it does not pad or block).
> - The function sets `is_reading` during the operation; concurrent calls while a read is in progress will return `KITTEN_FIFO_ERROR_ISREADING`.

Example:
```
uint16_t size = 0; // 0 means "read all available"
uint8_t buf[256];
kitten_fifo_error_t err = kitten_fifo_read_with_memcpy(&fifo, buf, &size);
// On success, err == KITTEN_FIFO_NOERROR and size is set to the number of bytes read
```
#### Zero-copy Read
Zero-copy read lets you access the FIFO internal buffer directly to avoid an extra `memcpy`, which can be faster for large or frequent reads.

Usage:
1. Call `kitten_fifo_read_with_nomemcpy(&fifo)` to acquire the read lock (`is_reading` set). If it returns an error, abort.
2. After processing the data directly from `fifo.buf` (may require two-part access), call `kitten_fifo_read_cpt(&fifo, size)` to commit how many bytes you consumed. `size` must be non-zero and no greater than the available bytes.

Notes:
- `kitten_fifo_read_with_nomemcpy` only acquires the read lock and does not modify `tail` or `used_size`.
- `kitten_fifo_read_cpt` updates `tail` and `used_size` and clears the read lock. It validates `size` and will return `KITTEN_FIFO_ERROR_ARGS` if `size == 0`, `KITTEN_FIFO_ERROR_NOTREAD` if no read is in progress, or `KITTEN_FIFO_ERROR_NODATA` if `size` exceeds available data.
- There is no separate cancel API: call `kitten_fifo_read_cpt` with the actual consumed bytes to release the lock.
- Concurrent reads are prevented by the `is_reading` flag; a second read attempt while one is active returns `KITTEN_FIFO_ERROR_ISREADING`.

Example:
```
kitten_fifo_error_t err = kitten_fifo_read_with_nomemcpy(&fifo);
// On success, err == KITTEN_FIFO_NOERROR
// Your read process here
kitten_fifo_read_cpt(&fifo, consumed);
```

### Error Codes
The library defines the following error codes (see `kitten_fifo.h` for full details):

- `KITTEN_FIFO_NOERROR`: No error; operation successful.
- `KITTEN_FIFO_ERROR_ARGS`: Missing or invalid function arguments.
- `KITTEN_FIFO_ERROR_CONFIG`: Invalid configuration (for example: null buffer, zero `fifo_size`, or missing IRQ callbacks in the config).
- `KITTEN_FIFO_ERROR_FIFO_NOT_INIT`: FIFO is not initialized. Call `kitten_fifo_init` first.
- `KITTEN_FIFO_ERROR_ISWRITING`: Cannot start a write operation because a write is already in progress (`is_writing` set).
- `KITTEN_FIFO_ERROR_NOTWRITTEN`: `kitten_fifo_write_cpt` was called without a preceding `kitten_fifo_write_with_nomemcpy` (no active write to commit).
- `KITTEN_FIFO_ERROR_ISREADING`: Cannot start a read operation because a read is already in progress (`is_reading` set).
- `KITTEN_FIFO_ERROR_NOTREAD`: `kitten_fifo_read_cpt` was called without a preceding `kitten_fifo_read_with_nomemcpy` (no active read to commit).
- `KITTEN_FIFO_ERROR_NOSPACE`: Not enough free space in the FIFO to write the requested number of bytes.
- `KITTEN_FIFO_ERROR_NODATA`: No data available to read from the FIFO.

![logo](./readme-pic/kitten-yyds.png)