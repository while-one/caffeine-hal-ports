#ifndef __CORE_CM4_H_GENERIC
#define __CORE_CM4_H_GENERIC

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define __IM            volatile const /*! Defines 'read only' structure member permissions */
#define __OM            volatile       /*! Defines 'write only' structure member permissions */
#define __IOM           volatile       /*! Defines 'read / write' structure member permissions */
#define __I             volatile const /*! Defines 'read only' permissions */
#define __O             volatile       /*! Defines 'write only' permissions */
#define __IO            volatile       /*! Defines 'read / write' permissions */
#define __STATIC_INLINE static inline

/* Memory mapping of Cortex-M4 Hardware */
#define SCS_BASE  (0xE000E000UL)        /*!< System Control Space Base Address */
#define NVIC_BASE (SCS_BASE + 0x0100UL) /*!< NVIC Base Address */
#define SCB_BASE  (SCS_BASE + 0x0D00UL) /*!< System Control Block Base Address */

/* SysTick */
typedef struct
{
    __IOM uint32_t CTRL;  /*!< Offset: 0x000 (R/W)  SysTick Control and Status Register */
    __IOM uint32_t LOAD;  /*!< Offset: 0x004 (R/W)  SysTick Reload Value Register */
    __IOM uint32_t VAL;   /*!< Offset: 0x008 (R/W)  SysTick Current Value Register */
    __IM uint32_t  CALIB; /*!< Offset: 0x00C (R/ )  SysTick Calibration Register */
} SysTick_Type;

/* NVIC */
typedef struct
{
    __IOM uint32_t ISER[8U]; /*!< Offset: 0x000 (R/W)  Interrupt Set Enable Register */
    uint32_t       RESERVED0[24U];
    __IOM uint32_t ICER[8U]; /*!< Offset: 0x080 (R/W)  Interrupt Clear Enable Register */
    uint32_t       RESERVED1[24U];
    __IOM uint32_t ISPR[8U]; /*!< Offset: 0x100 (R/W)  Interrupt Set Pending Register */
    uint32_t       RESERVED2[24U];
    __IOM uint32_t ICPR[8U]; /*!< Offset: 0x180 (R/W)  Interrupt Clear Pending Register */
    uint32_t       RESERVED3[24U];
    __IOM uint32_t IABR[8U]; /*!< Offset: 0x200 (R/W)  Interrupt Active bit Register */
    uint32_t       RESERVED4[56U];
    __IOM uint8_t  IP[240U]; /*!< Offset: 0x300 (R/W)  Interrupt Priority Register (8Bit wide) */
    uint32_t       RESERVED5[644U];
    __OM uint32_t  STIR; /*!< Offset: 0xE00 ( /W)  Software Trigger Interrupt Register */
} NVIC_Type;

#define SysTick ((SysTick_Type *) SysTick_BASE) /*!< SysTick configuration struct */
#define NVIC    ((NVIC_Type *) NVIC_BASE)       /*!< NVIC configuration struct */

#define __disable_irq()
#define __enable_irq()
#define __DSB()
#define __ISB()
#define __DMB()

#define __RBIT(x)      (x)
#define __CLZ(x)       (0)
#define __REV(x)       (0)
#define __REV16(x)     (0)
#define __REVSH(x)     (0)
#define __ROR(x, y)    (x)
#define __RBIT(x)      (x)
#define __LDREXW(x)    (*(x))
#define __STREXW(x, y) (*(y) = (x), 0)

#define __NVIC_EnableIRQ(IRQn)
#define __NVIC_DisableIRQ(IRQn)
#define __NVIC_SetPriority(IRQn, priority)

#ifdef __cplusplus
}
#endif

#endif /* __CORE_CM4_H_GENERIC */
