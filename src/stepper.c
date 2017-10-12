#include "stepper.h"

/*******************************************************************************
* Private Defines
*******************************************************************************/
#define MAX_STEPPERS 2

/*******************************************************************************
* Private Typedefs
*******************************************************************************/
typedef enum stepper_status_t {
  STEPPER_STATUS_INACTIVE,
  STEPPER_STATUS_ACTIVE
} stepper_status_t;

typedef struct stepper_t {
  uint8_t *dir_port;
  uint8_t *dir_port_ddr;
  uint8_t dir_pin;

  uint8_t *enable_port;
  uint8_t *enable_port_ddr;
  uint8_t enable_pin;

  uint8_t *step_port;
  uint8_t *step_port_ddr;
  uint8_t step_pin;

  stepper_status_t status;
  uint8_t speed;
} stepper_t;
/*******************************************************************************
* Private Data
*******************************************************************************/
static stepper_t steppers[MAX_STEPPERS];
/*******************************************************************************
* Public Function Definitions
*******************************************************************************/
stepper_err_t stepper_construct(
  stepper_attr_t config,
  stepper_descriptor_t *handle
) {
  uint8_t i;
  stepper_err_t err = STEPPER_ERR_NONE_AVAILABLE;

  for (i=0;i<MAX_STEPPERS;i++) {
    if (steppers[i].status == STEPPER_STATUS_INACTIVE) {
      steppers[i].dir_port = config.dir_port;
      steppers[i].dir_port_ddr = config.dir_port_ddr;
      steppers[i].dir_pin = config.dir_pin;

      steppers[i].enable_port = config.enable_port;
      steppers[i].enable_port_ddr = config.enable_port_ddr;
      steppers[i].enable_pin = config.enable_pin;

      steppers[i].step_port = config.step_port;
      steppers[i].step_port_ddr = config.step_port_ddr;
      steppers[i].step_pin = config.step_pin;

      *steppers[i].dir_port = 0;
      *steppers[i].dir_port_ddr |= steppers[i].dir_pin;

      *steppers[i].enable_port = 0;
      *steppers[i].enable_port_ddr |= steppers[i].enable_pin;

      *steppers[i].step_port = 0;
      *steppers[i].step_port_ddr |= steppers[i].step_pin;

      steppers[i].status = STEPPER_STATUS_ACTIVE;
      steppers[i].speed = config.speed;

      *handle = i;

      err = STEPPER_ERR_NONE;
      break;
    }
  }

  return err;
}

void stepper_destruct(stepper_descriptor_t handle) {
  steppers[handle].status = STEPPER_STATUS_INACTIVE;

}

stepper_err_t stepper_setSpeed(stepper_descriptor_t handle, uint8_t speed) {
  stepper_err_t err = STEPPER_ERR_NONE;

  if (handle >= MAX_STEPPERS
    || steppers[handle].status != STEPPER_STATUS_ACTIVE
  ) {
    err = STEPPER_ERR_HANDLE_INVALID;
  } else {
    steppers[handle].speed = speed;
  }
  return err;
}

uint8_t stepper_getSpeed(stepper_descriptor_t handle) {
  return steppers[handle].speed;
}
