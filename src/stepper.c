#include "stepper.h"

/*******************************************************************************
* Private Defines
*******************************************************************************/
#define MAX_STEPPERS 2
#define MAX_STEPPER_POS 199

/*******************************************************************************
* Private Typedefs
*******************************************************************************/
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

  uint8_t *ms1_port;
  uint8_t *ms1_port_ddr;
  uint8_t ms1_pin;

  uint8_t *ms2_port;
  uint8_t *ms2_port_ddr;
  uint8_t ms2_pin;

  uint8_t *ms3_port;
  uint8_t *ms3_port_ddr;
  uint8_t ms3_pin;

  stepper_status_t status;
  uint8_t speed;
  stepper_step_size_t step_size;
  uint8_t desired_pos_1;
  uint8_t desired_pos_2;
  uint8_t pos;
  stepper_dir_t dir;
  stepper_mode_t mode;
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
    if (steppers[i].status == STEPPER_STATUS_AVAILABLE) {
      steppers[i].dir_port = config.dir_port;
      steppers[i].dir_port_ddr = config.dir_port_ddr;
      steppers[i].dir_pin = config.dir_pin;

      steppers[i].enable_port = config.enable_port;
      steppers[i].enable_port_ddr = config.enable_port_ddr;
      steppers[i].enable_pin = config.enable_pin;

      steppers[i].step_port = config.step_port;
      steppers[i].step_port_ddr = config.step_port_ddr;
      steppers[i].step_pin = config.step_pin;

      steppers[i].ms1_port = config.ms1_port;
      steppers[i].ms1_port_ddr = config.ms1_port_ddr;
      steppers[i].ms1_pin = config.ms1_pin;

      steppers[i].ms2_port = config.ms2_port;
      steppers[i].ms2_port_ddr = config.ms2_port_ddr;
      steppers[i].ms2_pin = config.ms2_pin;

      steppers[i].ms3_port = config.ms3_port;
      steppers[i].ms3_port_ddr = config.ms3_port_ddr;
      steppers[i].ms3_pin = config.ms3_pin;

      *steppers[i].dir_port &= ~(1 << steppers[i].dir_pin);
      *steppers[i].dir_port_ddr |= (1 << steppers[i].dir_pin);
      // *steppers[i].dir_port_ddr |= steppers[i].dir_pin;

      // this pin is active low
      // *steppers[i].enable_port = 0;
      *steppers[i].enable_port |= (1 << steppers[i].enable_pin);
      *steppers[i].enable_port_ddr |= (1 << steppers[i].enable_pin);

      *steppers[i].step_port &= ~(1 << steppers[i].step_pin);
      *steppers[i].step_port_ddr |= (1 << steppers[i].step_pin);

      *steppers[i].ms1_port &= ~(1 << steppers[i].ms1_pin);
      *steppers[i].ms1_port_ddr |= (1 << steppers[i].ms1_pin);

      *steppers[i].ms2_port &= ~(1 << steppers[i].ms2_pin);
      *steppers[i].ms2_port_ddr |= (1 << steppers[i].ms2_pin);

      *steppers[i].ms3_port &= ~(1 << steppers[i].ms3_pin);
      *steppers[i].ms3_port_ddr |= (1 << steppers[i].ms3_pin);

      steppers[i].status = STEPPER_STATUS_DISABLED;
      steppers[i].speed = config.speed;
      steppers[i].desired_pos_1 = 0;
      steppers[i].desired_pos_2 = 0;
      steppers[i].pos = 0;

      *handle = i;

      err = STEPPER_ERR_NONE;
      break;
    }
  }

  return err;
}

void stepper_destruct(stepper_descriptor_t handle) {
  steppers[handle].status = STEPPER_STATUS_AVAILABLE;
}

stepper_err_t stepper_enable(stepper_descriptor_t handle) {
  stepper_err_t err = STEPPER_ERR_NONE;

  if (handle >= MAX_STEPPERS) {
    err = STEPPER_ERR_HANDLE_INVALID;
  } else {
    steppers[handle].status = STEPPER_STATUS_ENABLED;
    *steppers[handle].enable_port &= ~(1 << steppers[handle].enable_pin);
  }

  return err;
}

stepper_err_t stepper_disable(stepper_descriptor_t handle) {
  stepper_err_t err = STEPPER_ERR_NONE;

  if (handle >= MAX_STEPPERS
    || steppers[handle].status == STEPPER_STATUS_AVAILABLE
  ) {
    err = STEPPER_ERR_HANDLE_INVALID;
  } else {
    steppers[handle].status = STEPPER_STATUS_DISABLED;
    *steppers[handle].enable_port |= (1 << steppers[handle].enable_pin);
  }

  return err;
}

stepper_status_t stepper_getStatus(stepper_descriptor_t handle) {
  return steppers[handle].status;
}

stepper_err_t stepper_setSpeed(stepper_descriptor_t handle, uint8_t speed) {
  stepper_err_t err = STEPPER_ERR_NONE;

  if (handle >= MAX_STEPPERS
    || steppers[handle].status == STEPPER_STATUS_AVAILABLE
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

stepper_err_t stepper_setStepSize(
  stepper_descriptor_t handle,
  stepper_step_size_t step_size
) {
  stepper_err_t err = STEPPER_ERR_NONE;

  if (handle >= MAX_STEPPERS
    || steppers[handle].status == STEPPER_STATUS_AVAILABLE
  ) {
    err = STEPPER_ERR_HANDLE_INVALID;
  } else {
    // set ms1
    if (step_size == STEPPER_STEP_SIZE_HALF
      || step_size == STEPPER_STEP_SIZE_EIGHTH
      || step_size == STEPPER_STEP_SIZE_SIXTEENTH
    ) {
      *steppers[handle].ms1_port |= (1 << steppers[handle].ms1_pin);
    } else {
      *steppers[handle].ms1_port &= ~(1 << steppers[handle].ms1_pin);
    }

    // set ms2
    if (step_size == STEPPER_STEP_SIZE_QUARTER
      || step_size == STEPPER_STEP_SIZE_EIGHTH
      || step_size == STEPPER_STEP_SIZE_SIXTEENTH
    ) {
      *steppers[handle].ms2_port |= (1 << steppers[handle].ms2_pin);
    } else {
      *steppers[handle].ms2_port &= ~(1 << steppers[handle].ms2_pin);
    }

    // set ms3
    if (step_size == STEPPER_STEP_SIZE_SIXTEENTH
    ) {
      *steppers[handle].ms3_port |= (1 << steppers[handle].ms3_pin);
    } else {
      *steppers[handle].ms3_port &= ~(1 << steppers[handle].ms3_pin);
    }

    steppers[handle].step_size = step_size;
  }
  return err;
}

stepper_step_size_t stepper_getStepSize(stepper_descriptor_t handle) {
  return steppers[handle].step_size;
}

stepper_err_t stepper_setPos(
  stepper_descriptor_t handle,
  uint8_t pos_1,
  uint8_t pos_2
) {
  stepper_err_t err = STEPPER_ERR_NONE;

  if (handle >= MAX_STEPPERS
    || steppers[handle].status == STEPPER_STATUS_AVAILABLE
  ) {
    err = STEPPER_ERR_HANDLE_INVALID;
  } else {
    if (pos_1 <= MAX_STEPPER_POS && pos_2 <= MAX_STEPPER_POS) {
      steppers[handle].desired_pos_1 = pos_1;
      steppers[handle].desired_pos_2 = pos_2;
    } else {
      err = STEPPER_ERR_POSITION_INVALID;
    }
  }
  return err;
}

uint8_t stepper_getPos( stepper_descriptor_t handle) {
  return steppers[handle].pos;
}

uint8_t stepper_getDesiredPos1(stepper_descriptor_t handle) {
  return steppers[handle].desired_pos_1;
}

uint8_t stepper_getDesiredPos2(stepper_descriptor_t handle) {
  return steppers[handle].desired_pos_2;
}

stepper_err_t stepper_setDir(stepper_descriptor_t handle, stepper_dir_t dir) {
  stepper_err_t err = STEPPER_ERR_NONE;

  if (handle >= MAX_STEPPERS
    || steppers[handle].status == STEPPER_STATUS_AVAILABLE
  ) {
    err = STEPPER_ERR_HANDLE_INVALID;
  } else {
    steppers[handle].dir = dir;
    if (dir == STEPPER_DIR_FORWARD) {
      *steppers[handle].dir_port &= ~(dir << steppers[handle].dir_pin);
    } else {
      *steppers[handle].dir_port |= (dir << steppers[handle].dir_pin);
    }
  }
  return err;
}

stepper_dir_t stepper_getDir(stepper_descriptor_t handle) {
  return steppers[handle].dir;
}


stepper_err_t stepper_stepEngage(stepper_descriptor_t handle) {
  stepper_err_t err = STEPPER_ERR_NONE;
  if (handle >= MAX_STEPPERS
    || steppers[handle].status == STEPPER_STATUS_AVAILABLE
  ) {
    err = STEPPER_ERR_HANDLE_INVALID;
  } else {
    // its not an error, but don't set the step bit if the stepper is disabled
    // or if there is no need for stepping
    if (steppers[handle].status == STEPPER_STATUS_ENABLED
      && steppers[handle].pos != steppers[handle].desired_pos_1
    ) {
      *steppers[handle].step_port |= (1 << steppers[handle].step_pin);
      if (steppers[handle].dir == STEPPER_DIR_FORWARD) {
        if (steppers[handle].pos == 199) {
          steppers[handle].pos = 0;
        } else {
          steppers[handle].pos++;
        }
      } else if (steppers[handle].dir == STEPPER_DIR_REVERSE) {
        if (steppers[handle].pos == 0) {
          steppers[handle].pos = 199;
        } else {
          steppers[handle].pos--;
        }
      }
    }
  }

  return err;
}

stepper_err_t stepper_stepRelease(stepper_descriptor_t handle) {
  stepper_err_t err = STEPPER_ERR_NONE;
  uint8_t temp;

  if (handle >= MAX_STEPPERS
    || steppers[handle].status == STEPPER_STATUS_AVAILABLE
  ) {
    err = STEPPER_ERR_HANDLE_INVALID;
  } else {
    *steppers[handle].step_port &= ~(1 << steppers[handle].step_pin);
    if (steppers[handle].desired_pos_1 == steppers[handle].pos
      && steppers[handle].mode == STEPPER_MODE_OSCILLATE
    ) {
      temp = steppers[handle].desired_pos_1;
      steppers[handle].desired_pos_1 = steppers[handle].desired_pos_2;
      steppers[handle].desired_pos_2 = temp;
      if (steppers[handle].dir == STEPPER_DIR_REVERSE) {
        steppers[handle].dir = STEPPER_DIR_FORWARD;
      } else if (steppers[handle].dir == STEPPER_DIR_FORWARD) {
        steppers[handle].dir = STEPPER_DIR_REVERSE;
      }
    }
  }

  return err;
}

stepper_err_t stepper_setMode(stepper_descriptor_t handle, stepper_mode_t mode) {
  stepper_err_t err = STEPPER_ERR_NONE;

  if (handle >= MAX_STEPPERS
    || steppers[handle].status == STEPPER_STATUS_AVAILABLE
  ) {
    err = STEPPER_ERR_HANDLE_INVALID;
  } else if (mode != STEPPER_MODE_NORMAL && mode != STEPPER_MODE_OSCILLATE) {
    err = STEPPER_ERR_OPTION_INVALID;
  } else {
    steppers[handle].mode = mode;
  }

  return err;
}
stepper_mode_t stepper_getMode(stepper_descriptor_t handle) {
  return steppers[handle].mode;
}
