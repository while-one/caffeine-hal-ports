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
 * @file cfn_hal_rtc_port.c
 * @brief STM32F4 RTC HAL Port Implementation
 */

/* Includes ---------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cfn_hal_rtc.h"
#include "cfn_hal_rtc_port.h"
#include "cfn_hal_clock_port.h"
#include "cfn_hal_stm32_error.h"

#ifdef HAL_RTC_MODULE_ENABLED

/* Private Data -----------------------------------------------------*/

static RTC_TypeDef *const PORT_INSTANCES[CFN_HAL_RTC_PORT_MAX] = {
#if defined(RTC)
    [CFN_HAL_RTC_PORT_RTC1] = RTC,
#endif
};

static RTC_HandleTypeDef port_hrtcs[CFN_HAL_RTC_PORT_MAX];
static cfn_hal_rtc_t    *port_drivers[CFN_HAL_RTC_PORT_MAX];

/* Internal Helpers -------------------------------------------------*/

static uint32_t get_port_id_from_handle(RTC_HandleTypeDef *handle)
{
    if ((handle < &port_hrtcs[0]) || (handle >= &port_hrtcs[CFN_HAL_RTC_PORT_MAX]))
    {
        return UINT32_MAX;
    }
    return (uint32_t) (handle - port_hrtcs);
}

/* VMT Implementations ----------------------------------------------*/

static cfn_hal_error_code_t low_level_init(cfn_hal_rtc_t *driver)
{
    if ((driver == NULL) || (driver->phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    if (port_id >= CFN_HAL_RTC_PORT_MAX)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    /* Note: RTC clock enablement usually involves PWR and Backup Domain access */
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWR_EnableBkUpAccess();
    __HAL_RCC_RTC_ENABLE();

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_init(cfn_hal_driver_t *base)
{
    cfn_hal_rtc_t *driver = (cfn_hal_rtc_t *) base;

    cfn_hal_error_code_t err = cfn_hal_rtc_config_validate(driver->config);
    if (err != CFN_HAL_ERROR_OK)
    {
        return err;
    }

    if (driver->api->base.config_validate != NULL)
    {
        err = driver->api->base.config_validate((cfn_hal_driver_t *) driver, driver->config);
        if (err != CFN_HAL_ERROR_OK)
        {
            return err;
        }
    }

    err = low_level_init(driver);
    if (err != CFN_HAL_ERROR_OK)
    {
        return err;
    }

    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    RTC_HandleTypeDef *hrtc    = &port_hrtcs[port_id];

    hrtc->Instance = PORT_INSTANCES[port_id];
    hrtc->Init.HourFormat =
        (driver->config->format == CFN_HAL_RTC_CONFIG_FORMAT_12H) ? RTC_HOURFORMAT_12 : RTC_HOURFORMAT_24;
    hrtc->Init.AsynchPrediv   = 127;
    hrtc->Init.SynchPrediv    = 255;
    hrtc->Init.OutPut         = RTC_OUTPUT_DISABLE;
    hrtc->Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    hrtc->Init.OutPutType     = RTC_OUTPUT_TYPE_OPENDRAIN;

    return cfn_hal_stm32_map_error(HAL_RTC_Init(hrtc));
}

static cfn_hal_error_code_t port_base_deinit(cfn_hal_driver_t *base)
{
    cfn_hal_rtc_t *driver  = (cfn_hal_rtc_t *) base;
    uint32_t       port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_RTC_DeInit(&port_hrtcs[port_id]));
}

static cfn_hal_error_code_t port_base_config_set(cfn_hal_driver_t *base, const void *config)
{
    CFN_HAL_UNUSED(base);
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

/* ST HAL Callback Overrides ----------------------------------------*/

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
    uint32_t port_id = get_port_id_from_handle(hrtc);
    if ((port_id != UINT32_MAX) && (port_drivers[port_id] != NULL))
    {
        cfn_hal_rtc_t *driver = port_drivers[port_id];
        if (driver->cb != NULL)
        {
            driver->cb(driver, 0, 0, 0, driver->cb_user_arg); /* id 0 for Alarm A */
        }
    }
}

void HAL_RTCEx_AlarmBEventCallback(RTC_HandleTypeDef *hrtc)
{
    uint32_t port_id = get_port_id_from_handle(hrtc);
    if ((port_id != UINT32_MAX) && (port_drivers[port_id] != NULL))
    {
        cfn_hal_rtc_t *driver = port_drivers[port_id];
        if (driver->cb != NULL)
        {
            driver->cb(driver, 0, 0, 1, driver->cb_user_arg); /* id 1 for Alarm B */
        }
    }
}

/* Raw ISR Handlers -------------------------------------------------*/

#ifndef CFN_HAL_PORT_DISABLE_IRQ_RTC
void RTC_Alarm_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    HAL_RTC_AlarmIRQHandler(&port_hrtcs[CFN_HAL_RTC_PORT_RTC1]);
}
#endif /* CFN_HAL_PORT_DISABLE_IRQ_RTC */

/* RTC Specific Functions */

static cfn_hal_error_code_t port_rtc_set_time(cfn_hal_rtc_t *driver, cfn_hal_rtc_time_t *time)
{
    uint32_t        port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    RTC_TimeTypeDef s_time  = { 0 };
    RTC_DateTypeDef s_date  = { 0 };

    s_time.Hours            = (uint8_t) time->tm_hour;
    s_time.Minutes          = (uint8_t) time->tm_min;
    s_time.Seconds          = (uint8_t) time->tm_sec;
    s_time.DayLightSaving   = RTC_DAYLIGHTSAVING_NONE;
    s_time.StoreOperation   = RTC_STOREOPERATION_RESET;

    if (HAL_RTC_SetTime(&port_hrtcs[port_id], &s_time, RTC_FORMAT_BIN) != HAL_OK)
    {
        return CFN_HAL_ERROR_FAIL;
    }

    s_date.WeekDay = (uint8_t) time->tm_wday + 1; /* tm is 0-6, ST is 1-7 */
    s_date.Month   = (uint8_t) time->tm_mon + 1;  /* tm is 0-11, ST is 1-12 */
    s_date.Date    = (uint8_t) time->tm_mday;
    s_date.Year    = (uint8_t) (time->tm_year % 100); /* 2-digit year */

    return cfn_hal_stm32_map_error(HAL_RTC_SetDate(&port_hrtcs[port_id], &s_date, RTC_FORMAT_BIN));
}

static cfn_hal_error_code_t port_rtc_get_time(cfn_hal_rtc_t *driver, cfn_hal_rtc_time_t *time)
{
    uint32_t        port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    RTC_TimeTypeDef s_time  = { 0 };
    RTC_DateTypeDef s_date  = { 0 };

    if (HAL_RTC_GetTime(&port_hrtcs[port_id], &s_time, RTC_FORMAT_BIN) != HAL_OK)
    {
        return CFN_HAL_ERROR_FAIL;
    }
    if (HAL_RTC_GetDate(&port_hrtcs[port_id], &s_date, RTC_FORMAT_BIN) != HAL_OK)
    {
        return CFN_HAL_ERROR_FAIL;
    }

    time->tm_hour = s_time.Hours;
    time->tm_min  = s_time.Minutes;
    time->tm_sec  = s_time.Seconds;
    time->tm_wday = (int) s_date.WeekDay - 1;
    time->tm_mday = (int) s_date.Date;
    time->tm_mon  = (int) s_date.Month - 1;
    time->tm_year = 100 + (int) s_date.Year; /* Assumption: Year 20xx */

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_rtc_set_alarm(cfn_hal_rtc_t *driver, uint32_t id, cfn_hal_rtc_time_t *time)
{
    uint32_t         port_id         = (uint32_t) (uintptr_t) driver->phy->instance;
    RTC_AlarmTypeDef s_alarm         = { 0 };

    s_alarm.AlarmTime.Hours          = (uint8_t) time->tm_hour;
    s_alarm.AlarmTime.Minutes        = (uint8_t) time->tm_min;
    s_alarm.AlarmTime.Seconds        = (uint8_t) time->tm_sec;
    s_alarm.AlarmTime.SubSeconds     = 0;
    s_alarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    s_alarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
    s_alarm.AlarmMask                = RTC_ALARMMASK_NONE;
    s_alarm.AlarmSubSecondMask       = RTC_ALARMSUBSECONDMASK_ALL;
    s_alarm.AlarmDateWeekDaySel      = RTC_ALARMDATEWEEKDAYSEL_DATE;
    s_alarm.AlarmDateWeekDay         = (uint8_t) time->tm_mday;
    s_alarm.Alarm                    = (id == 0) ? RTC_ALARM_A : RTC_ALARM_B;

    return cfn_hal_stm32_map_error(HAL_RTC_SetAlarm_IT(&port_hrtcs[port_id], &s_alarm, RTC_FORMAT_BIN));
}

static cfn_hal_error_code_t
port_rtc_get_alarm(cfn_hal_rtc_t *driver, uint32_t id, bool *elapsed, cfn_hal_rtc_time_t *time)
{
    uint32_t         port_id  = (uint32_t) (uintptr_t) driver->phy->instance;
    RTC_AlarmTypeDef s_alarm  = { 0 };
    uint32_t         alarm_id = (id == 0) ? RTC_ALARM_A : RTC_ALARM_B;

    if (HAL_RTC_GetAlarm(&port_hrtcs[port_id], &s_alarm, alarm_id, RTC_FORMAT_BIN) != HAL_OK)
    {
        return CFN_HAL_ERROR_FAIL;
    }

    if (time != NULL)
    {
        time->tm_hour = s_alarm.AlarmTime.Hours;
        time->tm_min  = s_alarm.AlarmTime.Minutes;
        time->tm_sec  = s_alarm.AlarmTime.Seconds;
        time->tm_mday = s_alarm.AlarmDateWeekDay;
    }

    if (elapsed != NULL)
    {
        /* On STM32F4, we check the alarm flag in the status register */
        *elapsed = __HAL_RTC_ALARM_GET_FLAG(&port_hrtcs[port_id], (id == 0) ? RTC_FLAG_ALRAF : RTC_FLAG_ALRBF);
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_rtc_stop_alarm(cfn_hal_rtc_t *driver, uint32_t id)
{
    uint32_t port_id  = (uint32_t) (uintptr_t) driver->phy->instance;
    uint32_t alarm_id = (id == 0) ? RTC_ALARM_A : RTC_ALARM_B;
    return cfn_hal_stm32_map_error(HAL_RTC_DeactivateAlarm(&port_hrtcs[port_id], alarm_id));
}

/* API --------------------------------------------------------------*/
static const cfn_hal_rtc_api_t RTC_API = {
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
    .set_time = port_rtc_set_time,
    .get_time = port_rtc_get_time,
    .set_alarm = port_rtc_set_alarm,
    .get_alarm = port_rtc_get_alarm,
    .stop_alarm = port_rtc_stop_alarm
};

#endif /* HAL_RTC_MODULE_ENABLED */

/* Instantiation ----------------------------------------------------*/

cfn_hal_error_code_t
cfn_hal_rtc_construct(cfn_hal_rtc_t *driver, const cfn_hal_rtc_config_t *config, const cfn_hal_rtc_phy_t *phy)
{
#ifdef HAL_RTC_MODULE_ENABLED
    if ((driver == NULL) || (phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t port_id = (uint32_t) (uintptr_t) phy->instance;
    if (port_id >= CFN_HAL_RTC_PORT_MAX || PORT_INSTANCES[port_id] == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    driver->api                  = &RTC_API;
    driver->base.type            = CFN_HAL_PERIPHERAL_TYPE_RTC;
    driver->base.status          = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
    driver->config               = config;
    driver->phy                  = phy;

    port_hrtcs[port_id].Instance = PORT_INSTANCES[port_id];
    port_drivers[port_id]        = driver;

    return CFN_HAL_ERROR_OK;
#else
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(config);
    CFN_HAL_UNUSED(phy);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
#endif
}

cfn_hal_error_code_t cfn_hal_rtc_destruct(cfn_hal_rtc_t *driver)
{
#ifdef HAL_RTC_MODULE_ENABLED
    if (driver == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    if (port_id < CFN_HAL_RTC_PORT_MAX)
    {
        port_drivers[port_id] = NULL;
    }

    driver->api         = NULL;
    driver->base.type   = CFN_HAL_PERIPHERAL_TYPE_RTC;
    driver->base.status = CFN_HAL_DRIVER_STATUS_UNKNOWN;
    driver->config      = NULL;
    driver->phy         = NULL;

    return CFN_HAL_ERROR_OK;
#else
    CFN_HAL_UNUSED(driver);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
#endif
}
