#include "stepper.h"

/*******************************************************************************
* Public Function Definitions
*******************************************************************************/
void stepper_construct(stepper_attr_t config) {
  *config.dir_port = 0;
  *config.dir_port_ddr |= config.dir_pin;

  *config.enable_port = 0;
  *config.enable_port_ddr |= config.enable_pin;

  *config.step_port = 0;
  *config.step_port_ddr |= config.step_pin;
}
