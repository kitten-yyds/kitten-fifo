[English](./README.md)
# 一个适用于嵌入式系统的简单 FIFO 库
这是一个适用于嵌入式系统的简单 FIFO（先进先出）库。提供了简单的API用于创建和管理FIFO缓冲区。该库设计轻量高效，适用于资源有限的嵌入式系统。

## 特点
- 简单的API用于创建和管理FIFO缓冲区
- 轻量高效的
- 适用于资源有限的嵌入式系统
- 支持静态和动态内存分配

## 状态
已在无RTOS的STM32H750XBH6微处理器上完全测试。目前处于开发初期，可能存在bug，请自行承担风险。

## 使用方法
要使用该库，您需要按照以下步骤操作：
**包含库文件** -> **初始化FIFO缓冲区** -> **写入/读取FIFO**
### 包含库文件
您需要在项目中包含头文件`kitten_fifo.h`。
```
#include "kitten_fifo.h"
```
> 确认您的构建系统（如Makefile或CMake）将`kitten_fifo.c`添加到编译源文件中，并且`kitten_fifo.h`在您的包含路径中。

### 初始化FIFO缓冲区
您需要创建一个FIFO缓冲区，如下所示：
```
#define FIFO_SIZE 256
uint8_t fifo_buffer[FIFO_SIZE];
```
或者您可以使用动态内存分配：
```
#define FIFO_SIZE 256
uint8_t *fifo_buffer = (uint8_t *)malloc(FIFO_SIZE);
```
之后，您可以使用`kitten_fifo_init`函数初始化FIFO缓冲区：
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
> 注意：    
> - `fifo_size` 的类型为 `uint16_t`（最大 65535），请据此选择合适的大小。   
> - `fifo_error_handle` 是一个错误回调函数指针，目前暂时暂未使用。    
> - 关于 `irq_disable` 和 `irq_enable`函数，库会在需要保护临界区时调用这些函数，以避免数据撕裂。

### 写入FIFO
这里有两种写入数据到FIFO缓冲区的方式：
1. 普通写入（安全且简单）：`kitten_fifo_write_with_memcpy`
2. 零拷贝写入（高级且快速）：`kitten_fifo_write_with_nomemcpy` + `kitten_fifo_write_cpt`
#### 普通写入
函数`kitten_fifo_write_with_memcpy`使用`memcpy`将数据从源缓冲区复制到FIFO缓冲区。您需要提供三个参数：
1. `fifo`：指向您要操作的FIFO的指针。
2. `data`：指向源数据缓冲区的指针。
3. `size`：要写入的数据长度（单位：字节）
> 如果写入操作成功，它返回`KITTEN_FIFO_NOERROR`，否则返回错误代码。

示例：
```
kitten_fifo_error_t err = kitten_fifo_write_with_memcpy(&fifo, data, size);
// 如果成功, err == KITTEN_FIFO_NOERROR
```
#### 零拷贝写入
`kitten_fifo_write_with_nomemcpy` 仅获取写锁（设置 `is_writing`）；它不会复制数据或更新 `head`/`used_size`。您必须自行将数据写入 FIFO 缓冲区，然后调用 `kitten_fifo_write_cpt` 提交写入的字节数。

> 注意：
> - 在调用 `kitten_fifo_write_cpt` 时会检查可用空间；若空间不足返回 `KITTEN_FIFO_ERROR_NOSPACE`。
> - `kitten_fifo_write_cpt` 要求 `size > 0` 且不得超过当前可用空间；若 `size == 0` 会返回 `KITTEN_FIFO_ERROR_ARGS`。
> - 为避免长时间占用写锁，请在 `kitten_fifo_write_with_nomemcpy` 成功后尽快完成写入并调用 `kitten_fifo_write_cpt`。

示例：
```
kitten_fifo_error_t err = kitten_fifo_write_with_nomemcpy(&fifo);
// 如果成功，err == KITTEN_FIFO_NOERROR
// 写入数据
err = kitten_fifo_write_cpt(&fifo, to_write);
// 如果成功，err == KITTEN_FIFO_NOERROR
```
### 读取FIFO
这里有两种从FIFO缓冲区读取数据的方式：
1. 普通读取（安全且简单）：`kitten_fifo_read_with_memcpy`
2. 零拷贝读取（高级且快速）：`kitten_fifo_read_with_nomemcpy` + `kitten_fifo_read_cpt`
#### 普通读取
函数`kitten_fifo_read_with_memcpy`使用`memcpy`将数据从FIFO缓冲区复制到目标缓冲区。您需要提供三个参数：
1. `fifo`：指向您要操作的FIFO的指针。
2. `data`：指向目标缓冲区的指针。
3. `size`：指向 uint16_t 的指针，用作入/出参。调用时若 *size == 0 则读取所有可用字节，否则请求最多 *size 字节；返回时 *size 被设置为实际复制的字节数。
> 注意：
> - 如果 FIFO 缓冲区为空，函数返回 `KITTEN_FIFO_ERROR_NODATA`，并将 `*size` 设为 0。
> - 如果请求的 `*size` 大于可用字节数，函数仅复制可用的字节并相应更新 `*size`（不会填充或阻塞）。
> - 在操作期间函数会设置 `is_reading` 标志；若在读取进行时并发调用，函数将返回 `KITTEN_FIFO_ERROR_ISREADING`。

示例：
```
uint16_t size = 0; // 0 意味着 "读取所有可用字节"
uint8_t buf[256];
kitten_fifo_error_t err = kitten_fifo_read_with_memcpy(&fifo, buf, &size);
// 如果成功, err == KITTEN_FIFO_NOERROR，size将会被设置为读取的字节数
```
#### 零拷贝读取
零拷贝读取允许您直接访问 FIFO 的内部缓冲区以避免额外的 `memcpy`，对于大块或频繁的读取，这会更快。

使用方法：
1. 调用 `kitten_fifo_read_with_nomemcpy(&fifo)` 以获取读取锁（`is_reading` 被设置）。若返回错误则中止操作。
2. 读取完毕后，调用 `kitten_fifo_read_cpt(&fifo, size)` 提交你读取的大小。`size`不为0。

> 注意：
> - `kitten_fifo_read_with_nomemcpy` 仅获取读取锁，并不会修改 `tail` 或 `used_size`。
> - `kitten_fifo_read_cpt` 会更新 `tail` 和 `used_size` 并清除读取锁。
> - 并发读取由 `is_reading` 标志阻止；当已有读取在进行时再次尝试读取会返回 `KITTEN_FIFO_ERROR_ISREADING`。

示例：
```
kitten_fifo_error_t err = kitten_fifo_read_with_nomemcpy(&fifo);
// 如果成功，err == KITTEN_FIFO_NOERROR
// 在此处直接访问 fifo.buf（可能需要按两段读取）
err = kitten_fifo_read_cpt(&fifo, size);
// 如果成功，err == KITTEN_FIFO_NOERROR
```

### 错误代码
库定义了以下错误代码（详见 `kitten_fifo.h`）：
- `KITTEN_FIFO_NOERROR`: 无错误；操作成功。
- `KITTEN_FIFO_ERROR_ARGS`: 缺少或无效的函数参数。
- `KITTEN_FIFO_ERROR_CONFIG`: 配置无效（例如：空缓冲区、`fifo_size` 为 0，或配置中缺少 IRQ 回调）。
- `KITTEN_FIFO_ERROR_FIFO_NOT_INIT`: FIFO 未初始化。请先调用 `kitten_fifo_init`。
- `KITTEN_FIFO_ERROR_ISWRITING`: 不能开始写入操作，因为已有写入在进行（`is_writing` 已设置）。
- `KITTEN_FIFO_ERROR_NOTWRITTEN`: 在没有先前调用 `kitten_fifo_write_with_nomemcpy` 的情况下调用了 `kitten_fifo_write_cpt`（没有可提交的写入）。
- `KITTEN_FIFO_ERROR_ISREADING`: 不能开始读取操作，因为已有读取在进行（`is_reading` 已设置）。
- `KITTEN_FIFO_ERROR_NOTREAD`: 在没有先前调用 `kitten_fifo_read_with_nomemcpy` 的情况下调用了 `kitten_fifo_read_cpt`（没有可提交的读取）。
- `KITTEN_FIFO_ERROR_NOSPACE`: FIFO 中没有足够的可用空间来写入请求的字节数。
- `KITTEN_FIFO_ERROR_NODATA`: 没有可供读取的数据。

![logo](./readme-pic/kitten-yyds.png)
