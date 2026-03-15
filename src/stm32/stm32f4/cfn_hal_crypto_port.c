/**
 * @file cfn_hal_crypto_port.c
 * @brief STM32F4 Hardware Crypto HAL Port Implementation.
 */

/* Includes ---------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cfn_hal_crypto.h"
#include "cfn_hal_crypto_port.h"
#include "cfn_hal_stm32_error.h"
#include <string.h>

/* Private Data -----------------------------------------------------*/

static CRYP_HandleTypeDef port_hcryp;
static HASH_HandleTypeDef port_hhash;
static RNG_HandleTypeDef  port_hrng;

/* VMT Implementations ----------------------------------------------*/

static cfn_hal_error_code_t port_base_init(cfn_hal_driver_t *base)
{
    CFN_HAL_UNUSED(base);

    __HAL_RCC_CRYP_CLK_ENABLE();
    __HAL_RCC_HASH_CLK_ENABLE();
    __HAL_RCC_RNG_CLK_ENABLE();

    port_hcryp.Instance = CRYP;
    port_hcryp.Init.DataType = CRYP_DATATYPE_8B;
    port_hcryp.Init.KeySize = CRYP_KEYSIZE_128B;
    port_hcryp.Init.Algorithm = CRYP_AES_ECB;

    if (HAL_CRYP_Init(&port_hcryp) != HAL_OK)
    {
        return CFN_HAL_ERROR_FAIL;
    }

    /* HASH has no Instance member in F4 handle */
    if (HAL_HASH_Init(&port_hhash) != HAL_OK)
    {
        return CFN_HAL_ERROR_FAIL;
    }

    port_hrng.Instance = RNG;
    if (HAL_RNG_Init(&port_hrng) != HAL_OK)
    {
        return CFN_HAL_ERROR_FAIL;
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_deinit(cfn_hal_driver_t *base)
{
    CFN_HAL_UNUSED(base);
    (void) HAL_CRYP_DeInit(&port_hcryp);
    (void) HAL_HASH_DeInit(&port_hhash);
    (void) HAL_RNG_DeInit(&port_hrng);
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_power_state_set(cfn_hal_driver_t *base, cfn_hal_power_state_t state)
{
    CFN_HAL_UNUSED(base);
    CFN_HAL_UNUSED(state);
    return CFN_HAL_ERROR_OK;
}
static cfn_hal_error_code_t port_base_config_set(cfn_hal_driver_t *base, const void *config)
{
    CFN_HAL_UNUSED(config);
    return port_base_init(base);
}
static cfn_hal_error_code_t
port_base_callback_register(cfn_hal_driver_t *base, cfn_hal_callback_t callback, void *user_arg)
{
    CFN_HAL_UNUSED(base);
    CFN_HAL_UNUSED(callback);
    CFN_HAL_UNUSED(user_arg);
    return CFN_HAL_ERROR_OK;
}
static cfn_hal_error_code_t port_base_event_enable(cfn_hal_driver_t *base, uint32_t event_mask)
{
    CFN_HAL_UNUSED(base);
    CFN_HAL_UNUSED(event_mask);
    return CFN_HAL_ERROR_OK;
}
static cfn_hal_error_code_t port_base_event_disable(cfn_hal_driver_t *base, uint32_t event_mask)
{
    CFN_HAL_UNUSED(base);
    CFN_HAL_UNUSED(event_mask);
    return CFN_HAL_ERROR_OK;
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
static cfn_hal_error_code_t port_base_error_enable(cfn_hal_driver_t *base, uint32_t error_mask)
{
    CFN_HAL_UNUSED(base);
    CFN_HAL_UNUSED(error_mask);
    return CFN_HAL_ERROR_OK;
}
static cfn_hal_error_code_t port_base_error_disable(cfn_hal_driver_t *base, uint32_t error_mask)
{
    CFN_HAL_UNUSED(base);
    CFN_HAL_UNUSED(error_mask);
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
    return cfn_hal_stm32_map_error(
        HAL_CRYP_Encrypt(&port_hcryp,
                         (uint32_t *) (uintptr_t) /* NOLINT(performance-no-int-to-ptr) */ in,
                         (uint16_t) size,
                         (uint32_t *) (uintptr_t) /* NOLINT(performance-no-int-to-ptr) */ out,
                         100));
}

static cfn_hal_error_code_t port_crypto_decrypt(cfn_hal_crypto_t *driver, const uint8_t *in, uint8_t *out, size_t size)
{
    CFN_HAL_UNUSED(driver);
    return cfn_hal_stm32_map_error(
        HAL_CRYP_Decrypt(&port_hcryp,
                         (uint32_t *) (uintptr_t) /* NOLINT(performance-no-int-to-ptr) */ in,
                         (uint16_t) size,
                         (uint32_t *) (uintptr_t) /* NOLINT(performance-no-int-to-ptr) */ out,
                         100));
}

static cfn_hal_error_code_t port_crypto_hash_update(cfn_hal_crypto_t *driver, const uint8_t *data, size_t size)
{
    CFN_HAL_UNUSED(driver);
    return cfn_hal_stm32_map_error(HAL_HASHEx_SHA256_Accmlt(&port_hhash, (uint8_t *) data, (uint32_t) size));
}

static cfn_hal_error_code_t port_crypto_hash_finish(cfn_hal_crypto_t *driver, uint8_t *hash)
{
    CFN_HAL_UNUSED(driver);
    return cfn_hal_stm32_map_error(HAL_HASHEx_SHA256_Finish(&port_hhash, hash, 100));
}

static cfn_hal_error_code_t port_crypto_generate_random(cfn_hal_crypto_t *driver, uint8_t *buffer, size_t size)
{
    CFN_HAL_UNUSED(driver);
    uint32_t random_val = 0;
    size_t   i = 0;

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
}

static cfn_hal_error_code_t port_crypto_set_key(cfn_hal_crypto_t *driver, const uint8_t *key, size_t key_size)
{
    CFN_HAL_UNUSED(driver);
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
}

/* API --------------------------------------------------------------*/
static const cfn_hal_crypto_api_t CRYPTO_API = {
    .base = {
        .init = port_base_init,
        .deinit = port_base_deinit,
        .power_state_set = port_base_power_state_set,
        .config_set = port_base_config_set,
        .callback_register = port_base_callback_register,
        .event_enable = port_base_event_enable,
        .event_disable = port_base_event_disable,
        .event_get = port_base_event_get,
        .error_enable = port_base_error_enable,
        .error_disable = port_base_error_disable,
        .error_get = port_base_error_get,
    },
    .encrypt = port_crypto_encrypt,
    .decrypt = port_crypto_decrypt,
    .hash_update = port_crypto_hash_update,
    .hash_finish = port_crypto_hash_finish,
    .generate_random = port_crypto_generate_random,
    .set_key = port_crypto_set_key
};

/* Instantiation ----------------------------------------------------*/
cfn_hal_error_code_t cfn_hal_crypto_construct(cfn_hal_crypto_t              *driver,
                                              const cfn_hal_crypto_config_t *config,
                                              const cfn_hal_crypto_phy_t    *phy)
{
    if ((driver == NULL) || (phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->api = &CRYPTO_API;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_CRYPTO;
    driver->base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
    driver->config = config;
    driver->phy = phy;
    return CFN_HAL_ERROR_OK;
}

cfn_hal_error_code_t cfn_hal_crypto_destruct(cfn_hal_crypto_t *driver)
{
    if (driver == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->api = NULL;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_CRYPTO;
    driver->base.status = CFN_HAL_DRIVER_STATUS_UNKNOWN;
    driver->config = NULL;
    driver->phy = NULL;
    return CFN_HAL_ERROR_OK;
}
