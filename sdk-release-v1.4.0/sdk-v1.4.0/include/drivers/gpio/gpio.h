#ifndef _DRIVER_GPIO_H
#define _DRIVER_GPIO_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------------------------------------------------*/
/*  typedef                                                                  */
/* --------------------------------------------------------------------------*/
enum _gpio_drive_mode_t
{
    GPIO_DM_INPUT,
    GPIO_DM_INPUT_PULL_DOWN,
    GPIO_DM_INPUT_PULL_UP,
    GPIO_DM_OUTPUT,
};
typedef int32_t gpio_drive_mode_t;

enum _gpio_intr_mode_t {
    GPIO_NO_INTR = 0,
    GPIO_INTR_POS_EDGE,
    GPIO_INTR_NEG_EDGE,
    GPIO_INTR_BOTH_EDGE,
    GPIO_INTR_LOW_LEVEL,
    GPIO_INTR_HIGH_LEVEL,
};
typedef int32_t gpio_intr_mode_t;

enum _gpio_pin_value_t
{
    GPIO_PV_LOW = 0,
    GPIO_PV_HIGH
};
typedef int32_t gpio_pin_value_t;


/* --------------------------------------------------------------------------*/
/*  MACROS                                                                   */
/* --------------------------------------------------------------------------*/
#define GPIO_PIN0       (0)
#define GPIO_PIN1       (1)
#define GPIO_PIN2       (2)
#define GPIO_PIN3       (3)
#define GPIO_PIN4       (4)
#define GPIO_PIN5       (5)
#define GPIO_PIN6       (6)
#define GPIO_PIN7       (7)
#define GPIO_PIN8       (8)
#define GPIO_PIN9       (9)
#define GPIO_PIN10      (10)
#define GPIO_PIN11      (11)
#define GPIO_PIN12      (12)
#define GPIO_PIN13      (13)
#define GPIO_PIN14      (14)
#define GPIO_PIN15      (15)
#define GPIO_PIN16      (16)
#define GPIO_PIN17      (17)
#define GPIO_PIN18      (18)
#define GPIO_PIN19      (19)
#define GPIO_PIN20      (20)
#define GPIO_PIN21      (21)
#define GPIO_PIN22      (22)
#define GPIO_PIN23      (23)
#define GPIO_PIN24      (24)
#define GPIO_PIN25      (25)
#define GPIO_PIN26      (26)
#define GPIO_PIN27      (27)
#define GPIO_PIN28      (28)
#define GPIO_PIN29      (29)
#define GPIO_PIN30      (30)
#define GPIO_PIN31      (31)

#define GPIO_MAX_PINNO  (32)


/* --------------------------------------------------------------------------*/
/** 
 * @brief  gpio_init 
 * 
 * @return   
 */
/* --------------------------------------------------------------------------*/
int gpio_init(void);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  gpio_set_drive_mode  set pin driver mode
 * 
 * @param pin
 * @param mode
 */
/* --------------------------------------------------------------------------*/
void gpio_set_drive_mode(uint8_t pin, gpio_drive_mode_t mode);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  gpio_get_pin         get gpio pin value
 * 
 * @param pin                   gpio pin number
 * 
 * @return   
 */
/* --------------------------------------------------------------------------*/
gpio_pin_value_t gpio_get_pin(uint8_t pin);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  gpio_set_pin         set gpio pin value
 * 
 * @param pin
 * @param value
 */
/* --------------------------------------------------------------------------*/
void gpio_set_pin(uint8_t pin, gpio_pin_value_t value);

typedef int (*gpio_irq_cb_t)(void *ctx);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  gpio_irq_register    register gpio irq function
 * 
 * @param gpio_pin              gpio pin
 * @param mode                  interrupt mode
 * @param cb                    callback function pointer
 * @param ctx                   callback context
 * 
 * @return   
 */
/* --------------------------------------------------------------------------*/
int gpio_irq_register(uint8_t gpio_pin, gpio_intr_mode_t mode, 
        gpio_irq_cb_t cb, void *ctx);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  gpio_enable_intr    enable interrupt for gpio pin
 * 
 * @param gpio_pin
 */
/* --------------------------------------------------------------------------*/
void gpio_enable_intr(uint8_t gpio_pin);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  gpio_disable_intr  disable interrupt for gpio pin
 * 
 * @param gpio_pin
 */
/* --------------------------------------------------------------------------*/
void gpio_disable_intr(uint8_t gpio_pin);

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_GPIO_H */

