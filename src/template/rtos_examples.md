/**
 * @file rtos_examples.md
 * @brief Examples of RTOS-specific locking implementations for Caffeine-HAL.
 *
 * This document provides boilerplate code for integrating Caffeine-HAL with
 * common Real-Time Operating Systems.
 */

# RTOS Locking Examples for Caffeine-HAL

Caffeine-HAL uses an opaque `lock_obj` pointer in the base driver structure and
two VMT hooks (`lock` and `unlock`) to provide thread safety.

## 1. FreeRTOS

In FreeRTOS, you typically use a Mutex Semaphore.

### Construction
```c
#include "FreeRTOS.h"
#include "semphr.h"

cfn_hal_error_code_t cfn_hal_uart_construct(cfn_hal_uart_t *driver, ...) {
    // ... other initialization ...
#if (CFN_HAL_USE_LOCK == 1)
    driver->base.lock_obj = xSemaphoreCreateMutex();
    if (driver->base.lock_obj == NULL) {
        return CFN_HAL_ERROR_MEMORY_ALLOC;
    }
#endif
    return CFN_HAL_ERROR_OK;
}
```

### VMT Implementation
```c
static cfn_hal_error_code_t port_base_lock(cfn_hal_driver_t *base, uint32_t timeout) {
    TickType_t xTicksToWait = (timeout == CFN_HAL_MAX_DELAY) ? portMAX_DELAY : pdMS_TO_TICKS(timeout);
    if (xSemaphoreTake((SemaphoreHandle_t)base->lock_obj, xTicksToWait) == pdTRUE) {
        return CFN_HAL_ERROR_OK;
    }
    return CFN_HAL_ERROR_BUSY; // Or TIMEOUT
}

static cfn_hal_error_code_t port_base_unlock(cfn_hal_driver_t *base) {
    xSemaphoreGive((SemaphoreHandle_t)base->lock_obj);
    return CFN_HAL_ERROR_OK;
}
```

## 2. Azure RTOS (ThreadX)

In ThreadX, use a `TX_MUTEX`. Since `lock_obj` is a pointer, you may need to
allocate the mutex structure or use a pointer to a static array of mutexes.

### Construction
```c
#include "tx_api.h"

cfn_hal_error_code_t cfn_hal_uart_construct(cfn_hal_uart_t *driver, ...) {
    // Note: ThreadX mutexes require a control block. 
    // You might want to include the TX_MUTEX in your extension struct.
    TX_MUTEX *mutex = malloc(sizeof(TX_MUTEX)); 
    if (tx_mutex_create(mutex, "cfn_uart_mutex", TX_NO_INHERIT) != TX_SUCCESS) {
        return CFN_HAL_ERROR_FAIL;
    }
    driver->base.lock_obj = mutex;
    return CFN_HAL_ERROR_OK;
}
```

### VMT Implementation
```c
static cfn_hal_error_code_t port_base_lock(cfn_hal_driver_t *base, uint32_t timeout) {
    uint32_t wait_option = (timeout == CFN_HAL_MAX_DELAY) ? TX_WAIT_FOREVER : (timeout * TX_TIMER_TICKS_PER_SECOND / 1000);
    if (tx_mutex_get((TX_MUTEX *)base->lock_obj, wait_option) == TX_SUCCESS) {
        return CFN_HAL_ERROR_OK;
    }
    return CFN_HAL_ERROR_BUSY;
}

static cfn_hal_error_code_t port_base_unlock(cfn_hal_driver_t *base) {
    tx_mutex_put((TX_MUTEX *)base->lock_obj);
    return CFN_HAL_ERROR_OK;
}
```

## 3. Zephyr RTOS

Zephyr provides a clean mutex API.

### Construction
```c
#include <zephyr/kernel.h>

cfn_hal_error_code_t cfn_hal_uart_construct(cfn_hal_uart_t *driver, ...) {
    struct k_mutex *mutex = k_malloc(sizeof(struct k_mutex));
    k_mutex_init(mutex);
    driver->base.lock_obj = mutex;
    return CFN_HAL_ERROR_OK;
}
```

### VMT Implementation
```c
static cfn_hal_error_code_t port_base_lock(cfn_hal_driver_t *base, uint32_t timeout) {
    k_timeout_t t = (timeout == CFN_HAL_MAX_DELAY) ? K_FOREVER : K_MSEC(timeout);
    if (k_mutex_lock((struct k_mutex *)base->lock_obj, t) == 0) {
        return CFN_HAL_ERROR_OK;
    }
    return CFN_HAL_ERROR_BUSY;
}

static cfn_hal_error_code_t port_base_unlock(cfn_hal_driver_t *base) {
    k_mutex_unlock((struct k_mutex *)base->lock_obj);
    return CFN_HAL_ERROR_OK;
}
```
