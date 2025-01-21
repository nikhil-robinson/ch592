/**
 * @file ultrasonic.h
 *
 * CH driver for ultrasonic range meters, e.g. HC-SR04, HY-SRF05 and so on
 */

#ifndef _CH_ULTRASONIC_H_
#define _CH_ULTRASONIC_H_

#include "CH59x_common.h"
#include "ch_common.h"


#ifdef __cplusplus
extern "C" {
#endif

#define CH_ERR_ULTRASONIC_PING         0x200
#define CH_ERR_ULTRASONIC_PING_TIMEOUT 0x201
#define CH_ERR_ULTRASONIC_ECHO_TIMEOUT 0x202


/**
 * Device descriptor
 */
typedef struct
{
  uint32_t trigger_pin;
  gpio_port_t trigger_gpio_port;
  uint32_t echo_pin;
  gpio_port_t echo_gpio_port;
} ultrasonic_sensor_t;

/**
 * Init ranging module
 * \param dev Pointer to the device descriptor
 */
void ultrasonic_init(const ultrasonic_sensor_t *dev);

/**
 * Measure distance
 * \param dev Pointer to the device descriptor
 * \param max_distance Maximal distance to measure, centimeters
 * \return Distance in centimeters or ULTRASONIC_ERROR_xxx if error occured
 */
int ultrasonic_measure_cm(const ultrasonic_sensor_t *dev, uint32_t max_distance, uint32_t *distance);

#ifdef __cplusplus
}
#endif

#endif /* _CH_ULTRASONIC_H_ */
