#ifndef _STEPPER_H
#define _STEPPER_H

#include <stdint.h>
/*******************************************************************************
* Public Typedefs
*******************************************************************************/
typedef struct stepper_attr_t {
  uint8_t *dir_port;
  uint8_t *dir_port_ddr;
  uint8_t dir_pin;

  uint8_t *enable_port;
  uint8_t *enable_port_ddr;
  uint8_t enable_pin;

  uint8_t *step_port;
  uint8_t *step_port_ddr;
  uint8_t step_pin;
} stepper_attr_t;

/*******************************************************************************
* Public Function Declarations
*******************************************************************************/
void stepper_construct(stepper_attr_t config);

#endif // _STEPPER_H
