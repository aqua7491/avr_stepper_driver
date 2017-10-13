#ifndef _STEPPER_H
#define _STEPPER_H

#include <stdint.h>
/*******************************************************************************
* Public Typedefs
*******************************************************************************/
typedef enum stepper_err_t {
  STEPPER_ERR_NONE,
  STEPPER_ERR_NONE_AVAILABLE,
  STEPPER_ERR_HANDLE_INVALID,
  STEPPER_ERR_POSITION_INVALID
} stepper_err_t;

typedef enum stepper_step_size_t {
  STEPPER_STEP_SIZE_FULL,
  STEPPER_STEP_SIZE_HALF,
  STEPPER_STEP_SIZE_QUARTER,
  STEPPER_STEP_SIZE_EIGHTH,
  STEPPER_STEP_SIZE_SIXTEENTH,
} stepper_step_size_t;

typedef enum stepper_dir_t {
  STEPPER_DIR_FORWARD,
  STEPPER_DIR_REVERSE
} stepper_dir_t;

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

  uint8_t *ms1_port;
  uint8_t *ms1_port_ddr;
  uint8_t ms1_pin;

  uint8_t *ms2_port;
  uint8_t *ms2_port_ddr;
  uint8_t ms2_pin;

  uint8_t *ms3_port;
  uint8_t *ms3_port_ddr;
  uint8_t ms3_pin;


  uint8_t speed;
} stepper_attr_t;

typedef uint8_t stepper_descriptor_t;

/*******************************************************************************
* Public Function Declarations
*******************************************************************************/
stepper_err_t stepper_construct(
  stepper_attr_t config,
  stepper_descriptor_t *handle
);
void stepper_destruct(stepper_descriptor_t handle);
stepper_err_t stepper_enable(stepper_descriptor_t handle);
stepper_err_t stepper_disable(stepper_descriptor_t handle);
stepper_err_t stepper_setSpeed(stepper_descriptor_t handle, uint8_t speed);
uint8_t stepper_getSpeed(stepper_descriptor_t handle);
stepper_err_t stepper_setStepSize(
  stepper_descriptor_t handle,
  stepper_step_size_t step_size
);
stepper_step_size_t stepper_getStepSize(stepper_descriptor_t handle);
stepper_err_t stepper_setPos(stepper_descriptor_t handle, uint8_t pos);
uint8_t stepper_getDesiredPos(stepper_descriptor_t handle);
stepper_err_t stepper_setDir(stepper_descriptor_t handle, stepper_dir_t dir);
stepper_dir_t stepper_getDir(stepper_descriptor_t handle);

stepper_err_t stepper_stepEngage(stepper_descriptor_t handle);

#endif // _STEPPER_H
