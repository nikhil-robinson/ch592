#include "ch_gpio.h"
#include "ch_spinlock.h"


#define GPIO_TAG "gpio"


typedef struct {
    gpio_isr_t fn;   /*!< isr function */
    void *args;      /*!< isr function args */
} gpio_isr_func_t;

typedef struct {
    spinlock_t gpio_spinlock;
    gpio_isr_func_t *gpio_isr_func;
} gpio_context_t;


/**
 * @brief Wrapper for CH_RETURN_ON_FALSE
 * 
 */
#define GPIO_CHECK(a, str, ret_val) CH_RETURN_ON_FALSE(a, ret_val, GPIO_TAG, "%s", str)


/**
 * @brief CHeck if the given gpio is valid or not
 * 
 */




static gpio_context_t gpio_context = {
    .gpio_spinlock = { ATOMIC_FLAG_INIT },
    .gpio_isr_func = NULL,
};

/**
 * @brief Set gpio A dirrection for the given gpio number.
 * 
 * @param pin pin number.
 * @param mode direction mode.
 */
static void gpioa_set_direction(uint32_t pin, gpio_mode_t mode)
{
    switch (mode)
    {
        case GPIO_MODE_INPUT:
            CH_LOGI(GPIO_TAG, "GPIOA Set input");
            R32_PA_DIR &= ~pin;
            break;
        case GPIO_MODE_OUTPUT:
            CH_LOGI(GPIO_TAG, "GPIOA Set output");
            R32_PA_PD_DRV |= pin;
            R32_PA_DIR |= pin;
            break;
        case GPIO_MODE_DISABLE:
            CH_LOGI(GPIO_TAG, "GPIOA Set disable");
            R32_PA_PD_DRV &= ~pin;
            R32_PA_PU &= ~pin;
            R32_PA_DIR &= ~pin;
            break;
        default:
            CH_LOGE(GPIO_TAG, "Invalid GPIO mode");
            break;
    }
}


/**
 * @brief Set gpio B dirrection for the given gpio number.
 * 
 * @param pin pin number.
 * @param mode direction mode.
 */
static void gpiob_set_direction(uint32_t pin, gpio_mode_t mode)
{
    switch (mode)
    {
        case GPIO_MODE_INPUT:
            R32_PB_DIR &= ~pin;
            break;
        case GPIO_MODE_OUTPUT:
            R32_PB_PD_DRV |= pin;
            R32_PB_DIR |= pin;
            break;
        case GPIO_MODE_DISABLE:
            R32_PB_PD_DRV &= ~pin;
            R32_PB_PU &= ~pin;
            R32_PB_DIR &= ~pin;
            break;
        default:
            CH_LOGE(GPIO_TAG, "Invalid GPIO mode");
            break;
    }
}


/**
 * @brief Set the direction of a giver GPIO port number.
 * 
 * @param gpio_num GPIO warper from gpio_num_t.
 * @param mode  GPIO direction.
 * @return ch_err_t 
 */
ch_err_t gpio_set_direction(gpio_num_t gpio_num, gpio_mode_t mode)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", CH_ERR_INVALID_ARG);
    if (gpio_num <= GPIOA_NUM_15)
    {
        CH_LOGI(GPIO_TAG, "GPIOA Set direction");
        uint32_t pin = GET_GPIO_MAP(GPIO_MAP_A(gpio_num));
        gpioa_set_direction(pin,mode);
    }
    else
    {
       CH_LOGI(GPIO_TAG, "GPIOB Set direction");
       uint32_t pin = GET_GPIO_MAP(GPIO_MAP_B(gpio_num));
       gpiob_set_direction(pin,mode);
    }
    return  CH_OK;
}


/**
 * @brief Set the level of gppio.
 * 
 * @param gpio_num 
 * @param level 
 * @return ch_err_t 
 */
ch_err_t gpio_set_level(gpio_num_t gpio_num, uint32_t level)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", CH_ERR_INVALID_ARG);
    if (gpio_num <= GPIOA_NUM_15)
    {
        uint32_t pin = GET_GPIO_MAP(GPIO_MAP_A(gpio_num));
        level ? GPIOA_SetBits(pin) : GPIOA_ResetBits(pin);
        return  CH_OK;
    }
    else
    {
       uint32_t pin = GET_GPIO_MAP(GPIO_MAP_B(gpio_num));
       level ? GPIOB_SetBits(pin) : GPIOB_ResetBits(pin);
       return  CH_OK;
    }
    return  CH_ERR_INVALID_ARG;
}

ch_err_t gpio_toggle(gpio_num_t gpio_num, bool level)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", CH_ERR_INVALID_ARG);
    if (gpio_num <= GPIOA_NUM_15)
    {
        uint32_t pin = GET_GPIO_MAP(GPIO_MAP_A(gpio_num));
        GPIOA_InverseBits(pin);
        return  CH_OK;
    }
    else
    {
       uint32_t pin = GET_GPIO_MAP(GPIO_MAP_B(gpio_num));
       GPIOB_InverseBits(pin);
       return  CH_OK;
    }
    return  CH_ERR_INVALID_ARG;
}

int gpio_get_level(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", CH_ERR_INVALID_ARG);
    if (gpio_num <= GPIOA_NUM_15)
    {
        uint32_t pin = GET_GPIO_MAP(GPIO_MAP_A(gpio_num));
        int val =  GPIOA_ReadPortPin(pin);
        return (val != 0);
    }
    else
    {
       uint32_t pin = GET_GPIO_MAP(GPIO_MAP_B(gpio_num));
       int val =  GPIOB_ReadPortPin(pin);
       return (val != 0);
    }
    return  0;
}

ch_err_t gpio_pullup_en(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", CH_ERR_INVALID_ARG);
    if (gpio_num <= GPIOA_NUM_15)
    {
        uint32_t pin = GET_GPIO_MAP(GPIO_MAP_A(gpio_num));
        R32_PA_PU |= pin;
        return  CH_OK;
    }
    else
    {
       uint32_t pin = GET_GPIO_MAP(GPIO_MAP_B(gpio_num));
       R32_PB_PU |= pin;
       return  CH_OK;
    }
    return  CH_ERR_INVALID_ARG;
}

ch_err_t gpio_pullup_dis(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", CH_ERR_INVALID_ARG);
    if (gpio_num <= GPIOA_NUM_15)
    {
        uint32_t pin = GET_GPIO_MAP(GPIO_MAP_A(gpio_num));
        R32_PA_PU &= ~pin;
        return  CH_OK;
    }
    else
    {
       uint32_t pin = GET_GPIO_MAP(GPIO_MAP_B(gpio_num));
       R32_PB_PU &= ~pin;
       return  CH_OK;
    }
    return  CH_ERR_INVALID_ARG;

}

ch_err_t gpio_pulldown_en(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", CH_ERR_INVALID_ARG);
    if (gpio_num <= GPIOA_NUM_15)
    {
        uint32_t pin = GET_GPIO_MAP(GPIO_MAP_A(gpio_num));
        R32_PA_PD_DRV |= pin;
        return  CH_OK;
    }
    else
    {
       uint32_t pin = GET_GPIO_MAP(GPIO_MAP_B(gpio_num));
       R32_PB_PD_DRV |= pin;
       return  CH_OK;
    }
    return  CH_ERR_INVALID_ARG;

}

ch_err_t gpio_pulldown_dis(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", CH_ERR_INVALID_ARG);
    if (gpio_num <= GPIOA_NUM_15)
    {
        uint32_t pin = GET_GPIO_MAP(GPIO_MAP_A(gpio_num));
        R32_PA_PD_DRV &= ~pin;
        return  CH_OK;
    }
    else
    {
       uint32_t pin = GET_GPIO_MAP(GPIO_MAP_B(gpio_num));
       R32_PB_PD_DRV &= ~pin;
       return  CH_OK;
    }
    return  CH_ERR_INVALID_ARG;

}

ch_err_t gpio_intr_enable(gpio_num_t gpio_num, gpio_int_type_t intr_type)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", CH_ERR_INVALID_ARG);
    if (gpio_num <= GPIOA_NUM_15)
    {
        uint32_t pin = GET_GPIO_MAP(GPIO_MAP_A(gpio_num));
        GPIOA_ITModeCfg(pin,intr_type);
        PFIC_EnableIRQ(GPIO_A_IRQn);
        return  CH_OK;
    }
    else
    {
       uint32_t pin = GET_GPIO_MAP(GPIO_MAP_B(gpio_num));
       GPIOB_ITModeCfg(pin,intr_type);
       PFIC_EnableIRQ(GPIO_B_IRQn);
       return  CH_OK;
    }
    return  CH_ERR_INVALID_ARG;

}


ch_err_t gpio_intr_disable(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", CH_ERR_INVALID_ARG);
    if (gpio_num <= GPIOA_NUM_15)
    {
        uint32_t pin = GET_GPIO_MAP(GPIO_MAP_A(gpio_num));
        PFIC_DisableIRQ(GPIO_A_IRQn);
        R16_PB_INT_EN &= ~pin;
        return  CH_OK;
    }
    else
    {
       uint32_t pin = GET_GPIO_MAP(GPIO_MAP_B(gpio_num));
       PFIC_DisableIRQ(GPIO_B_IRQn);
       R16_PB_INT_EN &= ~pin;
       return  CH_OK;
    }
    return  CH_ERR_INVALID_ARG;
}


ch_err_t gpio_reset_pin(gpio_num_t gpio_num)
{
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", CH_ERR_INVALID_ARG);
    gpio_intr_disable(gpio_num);
    if (gpio_num <= GPIOA_NUM_15)
    {
        uint32_t pin = GET_GPIO_MAP(GPIO_MAP_A(gpio_num));
        GPIOA_ModeCfg(pin,GPIO_MODE_DISABLE);
        return  CH_OK;
    }
    else
    {
       uint32_t pin = GET_GPIO_MAP(GPIO_MAP_B(gpio_num));
       GPIOB_ModeCfg(pin,GPIO_MODE_DISABLE);
       return  CH_OK;
    }
    return  CH_ERR_INVALID_ARG;

}


ch_err_t gpio_install_isr_service(int intr_alloc_flags)
{
    GPIO_CHECK(gpio_context.gpio_isr_func == NULL, "GPIO isr service already installed", CH_ERR_INVALID_STATE);
    ch_err_t ret = CH_ERR_NO_MEM;
    gpio_isr_func_t *isr_func = (gpio_isr_func_t *) malloc(GPIO_NUM_MAX * sizeof(gpio_isr_func_t));
    if (isr_func) {
        mutexENTER_CRITICAL(&gpio_context.gpio_spinlock);
        if (gpio_context.gpio_isr_func == NULL) {
            gpio_context.gpio_isr_func = isr_func;
            mutexEXIT_CRITICAL(&gpio_context.gpio_spinlock);
            if (ret != CH_OK) {
                // registering failed, uninstall isr service
                gpio_uninstall_isr_service();
            }
        } else {
            // isr service already installed, free allocated resource
            mutexEXIT_CRITICAL(&gpio_context.gpio_spinlock);
            ret = CH_ERR_INVALID_STATE;
            free(isr_func);
        }
    }

    return ret;
}

ch_err_t gpio_isr_handler_add(gpio_num_t gpio_num, gpio_isr_t isr_handler, void *args)
{
    GPIO_CHECK(gpio_context.gpio_isr_func != NULL, "GPIO isr service is not installed, call gpio_install_isr_service() first", CH_ERR_INVALID_STATE);
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", CH_ERR_INVALID_ARG);
    mutexENTER_CRITICAL(&gpio_context.gpio_spinlock);
    gpio_intr_disable(gpio_num);
    if (gpio_context.gpio_isr_func) {
        gpio_context.gpio_isr_func[gpio_num].fn = isr_handler;
        gpio_context.gpio_isr_func[gpio_num].args = args;
    }
    mutexEXIT_CRITICAL(&gpio_context.gpio_spinlock);
    return CH_OK;
}

ch_err_t gpio_isr_handler_remove(gpio_num_t gpio_num)
{
    GPIO_CHECK(gpio_context.gpio_isr_func != NULL, "GPIO isr service is not installed, call gpio_install_isr_service() first", CH_ERR_INVALID_STATE);
    GPIO_CHECK(GPIO_IS_VALID_GPIO(gpio_num), "GPIO number error", CH_ERR_INVALID_ARG);
    mutexENTER_CRITICAL(&gpio_context.gpio_spinlock);
    gpio_intr_disable(gpio_num);
    if (gpio_context.gpio_isr_func) {
        gpio_context.gpio_isr_func[gpio_num].fn = NULL;
        gpio_context.gpio_isr_func[gpio_num].args = NULL;
    }
    mutexEXIT_CRITICAL(&gpio_context.gpio_spinlock);
    return CH_OK;
}

void gpio_uninstall_isr_service(void)
{
    gpio_isr_func_t *gpio_isr_func_free = NULL;
    mutexENTER_CRITICAL(&gpio_context.gpio_spinlock);
    if (gpio_context.gpio_isr_func == NULL) {
        mutexEXIT_CRITICAL(&gpio_context.gpio_spinlock);
        return;
    }
    gpio_isr_func_free = gpio_context.gpio_isr_func;
    gpio_context.gpio_isr_func = NULL;
    mutexEXIT_CRITICAL(&gpio_context.gpio_spinlock);
    free(gpio_isr_func_free);
    return;
}



// __INTERRUPT
// __HIGH_CODE
// void GPIOA_IRQHandler(void)
// {
//     uint16_t flag;
//     flag = GPIOA_ReadITFlagPort();
//     GPIOA_ClearITFlagBit(flag);
// }

// __INTERRUPT
// __HIGH_CODE
// void GPIOB_IRQHandler(void)
// {
//     uint16_t flag;
//     flag = GPIOB_ReadITFlagPort();
//     GPIOB_ClearITFlagBit(flag);
// }