/**
 * Copyright (c) 2026 Hisham Moussa Daou <https://www.whileone.me>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file cfn_hal_crypto_port.c
 * @brief STM32F4 CRYPTO HAL Port Implementation
 */

/* Includes ---------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cfn_hal_crypto.h"
#include "cfn_hal_crypto_port.h"
#include "cfn_hal_stm32_error.h"
#include <string.h>

#if defined(HAL_CRYP_MODULE_ENABLED) || defined(HAL_HASH_MODULE_ENABLED) || defined(HAL_RNG_MODULE_ENABLED)

/* Private Data -----------------------------------------------------*/

#ifdef HAL_CRYP_MODULE_ENABLED
static CRYP_HandleTypeDef port_hcryp;
#endif

#ifdef HAL_HASH_MODULE_ENABLED
static HASH_HandleTypeDef port_hhash;
#endif

#ifdef HAL_RNG_MODULE_ENABLED
static RNG_HandleTypeDef port_hrng;
#endif

/* VMT Implementations ----------------------------------------------*/

static cfn_hal_error_code_t port_base_init(cfn_hal_driver_t *base)
{
    CFN_HAL_UNUSED(base);

#ifdef HAL_CRYP_MODULE_ENABLED
    __HAL_RCC_CRYP_CLK_ENABLE();
    port_hcryp.Instance       = CRYP;
    port_hcryp.Init.DataType  = CRYP_DATATYPE_8B;
    port_hcryp.Init.KeySize   = CRYP_KEYSIZE_128B;
    port_hcryp.Init.Algorithm = CRYP_AES_ECB;

    if (HAL_CRYP_Init(&port_hcryp) != HAL_OK)
    {
        return CFN_HAL_ERROR_FAIL;
    }
#endif

#ifdef HAL_HASH_MODULE_ENABLED
    __HAL_RCC_HASH_CLK_ENABLE();
    if (HAL_HASH_Init(&port_hhash) != HAL_OK)
    {
        return CFN_HAL_ERROR_FAIL;
    }
#endif

#ifdef HAL_RNG_MODULE_ENABLED
    __HAL_RCC_RNG_CLK_ENABLE();
    port_hrng.Instance = RNG;
    if (HAL_RNG_Init(&port_hrng) != HAL_OK)
    {
        return CFN_HAL_ERROR_FAIL;
    }
#endif

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_deinit(cfn_hal_driver_t *base)
{
    CFN_HAL_UNUSED(base);
#ifdef HAL_CRYP_MODULE_ENABLED
    (void) HAL_CRYP_DeInit(&port_hcryp);
#endif
#ifdef HAL_HASH_MODULE_ENABLED
    (void) HAL_HASH_DeInit(&port_hhash);
#endif
#ifdef HAL_RNG_MODULE_ENABLED
    (void) HAL_RNG_DeInit(&port_hrng);
#endif
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_config_set(cfn_hal_driver_t *base, const void *config)
{
    CFN_HAL_UNUSED(config);
    return port_base_init(base);
}

static cfn_hal_error_code_t port_base_event_get(cfn_hal_driver_t *base, uint32_t *event_mask)
{
    CFN_HAL_UNUSED(base);
    if (event_mask != NULL)
    {
        *event_mask = 0;
    }
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_error_get(cfn_hal_driver_t *base, uint32_t *error_mask)
{
    CFN_HAL_UNUSED(base);
    if (error_mask != NULL)
    {
        *error_mask = 0;
    }
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_crypto_encrypt(cfn_hal_crypto_t *driver, const uint8_t *in, uint8_t *out, size_t size)
{
    CFN_HAL_UNUSED(driver);
#ifdef HAL_CRYP_MODULE_ENABLED
    return cfn_hal_stm32_map_error(
        HAL_CRYP_Encrypt(&port_hcryp,
                         (uint32_t *) (uintptr_t) /* NOLINT(performance-no-int-to-ptr) */ in,
                         (uint16_t) size,
                         (uint32_t *) (uintptr_t) /* NOLINT(performance-no-int-to-ptr) */ out,
                         100));
#else
    CFN_HAL_UNUSED(in);
    CFN_HAL_UNUSED(out);
    CFN_HAL_UNUSED(size);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
#endif
}

static cfn_hal_error_code_t port_crypto_decrypt(cfn_hal_crypto_t *driver, const uint8_t *in, uint8_t *out, size_t size)
{
    CFN_HAL_UNUSED(driver);
#ifdef HAL_CRYP_MODULE_ENABLED
    return cfn_hal_stm32_map_error(
        HAL_CRYP_Decrypt(&port_hcryp,
                         (uint32_t *) (uintptr_t) /* NOLINT(performance-no-int-to-ptr) */ in,
                         (uint16_t) size,
                         (uint32_t *) (uintptr_t) /* NOLINT(performance-no-int-to-ptr) */ out,
                         100));
#else
    CFN_HAL_UNUSED(in);
    CFN_HAL_UNUSED(out);
    CFN_HAL_UNUSED(size);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
#endif
}

static cfn_hal_error_code_t port_crypto_hash_update(cfn_hal_crypto_t *driver, const uint8_t *data, size_t size)
{
    CFN_HAL_UNUSED(driver);
#ifdef HAL_HASH_MODULE_ENABLED
    return cfn_hal_stm32_map_error(HAL_HASHEx_SHA256_Accmlt(&port_hhash, (uint8_t *) data, (uint32_t) size));
#else
    CFN_HAL_UNUSED(data);
    CFN_HAL_UNUSED(size);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
#endif
}

static cfn_hal_error_code_t port_crypto_hash_finish(cfn_hal_crypto_t *driver, uint8_t *hash)
{
    CFN_HAL_UNUSED(driver);
#ifdef HAL_HASH_MODULE_ENABLED
    return cfn_hal_stm32_map_error(HAL_HASHEx_SHA256_Finish(&port_hhash, hash, 100));
#else
    CFN_HAL_UNUSED(hash);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
#endif
}

static cfn_hal_error_code_t port_crypto_generate_random(cfn_hal_crypto_t *driver, uint8_t *buffer, size_t size)
{
    CFN_HAL_UNUSED(driver);
#ifdef HAL_RNG_MODULE_ENABLED
    uint32_t random_val = 0;
    size_t   i          = 0;

    while (i < size)
    {
        if (HAL_RNG_GenerateRandomNumber(&port_hrng, &random_val) != HAL_OK)
        {
            return CFN_HAL_ERROR_FAIL;
        }

        size_t chunk = (size - i >= 4) ? 4 : (size - i);
        memcpy(&buffer[i], &random_val, chunk);
        i += chunk;
    }

    return CFN_HAL_ERROR_OK;
#else
    CFN_HAL_UNUSED(buffer);
    CFN_HAL_UNUSED(size);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
#endif
}

static cfn_hal_error_code_t port_crypto_set_key(cfn_hal_crypto_t *driver, const uint8_t *key, size_t key_size)
{
    CFN_HAL_UNUSED(driver);
#ifdef HAL_CRYP_MODULE_ENABLED
    port_hcryp.Init.pKey = (uint32_t *) (uintptr_t) /* NOLINT(performance-no-int-to-ptr) */ key;
    if (key_size == 16)
    {
        port_hcryp.Init.KeySize = CRYP_KEYSIZE_128B;
    }
    else if (key_size == 32)
    {
        port_hcryp.Init.KeySize = CRYP_KEYSIZE_256B;
    }
    else
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    return cfn_hal_stm32_map_error(HAL_CRYP_Init(&port_hcryp));
#else
    CFN_HAL_UNUSED(key);
    CFN_HAL_UNUSED(key_size);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
#endif
}

/* API --------------------------------------------------------------*/
static const cfn_hal_crypto_api_t CRYPTO_API = {
    .base = {
        .init = port_base_init,
        .deinit = port_base_deinit,
        .power_state_set = NULL,
        .config_set = port_base_config_set,
        .callback_register = NULL,
        .event_enable = NULL,
        .event_disable = NULL,
        .event_get = port_base_event_get,
        .error_enable = NULL,
        .error_disable = NULL,
        .error_get = port_base_error_get,
    },
    .encrypt = port_crypto_encrypt,
    .decrypt = port_crypto_decrypt,
    .hash_update = port_crypto_hash_update,
    .hash_finish = port_crypto_hash_finish,
    .generate_random = port_crypto_generate_random,
    .set_key = port_crypto_set_key
};

#endif /* HAL_CRYP_MODULE_ENABLED || HAL_HASH_MODULE_ENABLED || HAL_RNG_MODULE_ENABLED */

/* Instantiation ----------------------------------------------------*/
cfn_hal_error_code_t cfn_hal_crypto_construct(cfn_hal_crypto_t              *driver,
                                              const cfn_hal_crypto_config_t *config,
                                              const cfn_hal_crypto_phy_t    *phy)
{
#if defined(HAL_CRYP_MODULE_ENABLED) || defined(HAL_HASH_MODULE_ENABLED) || defined(HAL_RNG_MODULE_ENABLED)
    if ((driver == NULL) || (phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->api         = &CRYPTO_API;
    driver->base.type   = CFN_HAL_PERIPHERAL_TYPE_CRYPTO;
    driver->base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
    driver->config      = config;
    driver->phy         = phy;
    return CFN_HAL_ERROR_OK;
#else
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(config);
    CFN_HAL_UNUSED(phy);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
#endif
}

cfn_hal_error_code_t cfn_hal_crypto_destruct(cfn_hal_crypto_t *driver)
{
    if (driver == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->api         = NULL;
    driver->base.type   = CFN_HAL_PERIPHERAL_TYPE_CRYPTO;
    driver->base.status = CFN_HAL_DRIVER_STATUS_UNKNOWN;
    driver->config      = NULL;
    driver->phy         = NULL;
    return CFN_HAL_ERROR_OK;
}
