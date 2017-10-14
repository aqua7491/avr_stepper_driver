#include "unity.h"
#include <time.h>
#include <stdlib.h>
/*******************************************************************************
* Module Under Test
*******************************************************************************/
#include "stepper.h"

/*******************************************************************************
* Private Defines
*******************************************************************************/
#define MAX_STEPPERS 2
#define NUM_STEP_SIZE_OPTIONS 5
#define MAX_STEPPER_POS 199
#define NUM_DIRECTIONS 2

/*******************************************************************************
* Local Data
*******************************************************************************/
// GPIO
static uint8_t dir_port;
static uint8_t dir_port_ddr;
static uint8_t dir_pin;
static uint8_t enable_port;
static uint8_t enable_port_ddr;
static uint8_t enable_pin;
static uint8_t step_port;
static uint8_t step_port_ddr;
static uint8_t step_pin;

static uint8_t ms1_port;
static uint8_t ms1_port_ddr;
static uint8_t ms1_pin;
static uint8_t ms2_port;
static uint8_t ms2_port_ddr;
static uint8_t ms2_pin;
static uint8_t ms3_port;
static uint8_t ms3_port_ddr;
static uint8_t ms3_pin;

stepper_descriptor_t stepper_handles[2];
/*******************************************************************************
* Private Function Declarations
*******************************************************************************/
stepper_err_t _makeStepper(uint8_t handle_index);

/*******************************************************************************
* Setup and Teardown
*******************************************************************************/
void setUp(void)
{
  srand(time(NULL));

  dir_port = (uint8_t)rand();
  dir_port_ddr = 0;
  dir_pin = (rand() % 8);

  enable_port = (uint8_t)rand();
  enable_port_ddr = 0;
  enable_pin = (rand() % 8);

  step_port = (uint8_t)rand();
  step_port_ddr = 0;
  step_pin = (rand() % 8);

  ms1_port = (uint8_t)rand();
  ms1_port_ddr = 0;
  ms1_pin = (rand() % 8);

  ms2_port = (uint8_t)rand();
  ms2_port_ddr = 0;
  ms2_pin = (rand() % 8);

  ms3_port = (uint8_t)rand();
  ms3_port_ddr = 0;
  ms3_pin = (rand() % 8);
}

void tearDown(void)
{
  uint8_t i;
  for (i=0;i<MAX_STEPPERS;i++) {
    stepper_destruct(i);
  }
}

/*******************************************************************************
* Tests
*******************************************************************************/
void test_construct_initializes_hardware_pins_with_individual_ports(void)
{
  _makeStepper(0);

  TEST_ASSERT((dir_port & (1 << dir_pin)) == 0);
  TEST_ASSERT(dir_port_ddr & (1 << dir_pin));

  TEST_ASSERT(enable_port & (1 << enable_pin));
  TEST_ASSERT(enable_port_ddr & (1 << enable_pin));

  TEST_ASSERT((step_port & (1 << step_pin)) == 0);
  TEST_ASSERT(step_port_ddr & (1 << step_pin));

  TEST_ASSERT((ms1_port & (1 << ms1_pin)) == 0);
  TEST_ASSERT(ms1_port_ddr & (1 << ms1_pin));

  TEST_ASSERT((ms2_port & (1 << ms2_pin)) == 0);
  TEST_ASSERT(ms2_port_ddr & (1 << ms2_pin));

  TEST_ASSERT((ms3_port & (1 << ms3_pin)) == 0);
  TEST_ASSERT(ms3_port_ddr & (1 << ms3_pin));
}

void test_construct_initializes_hardware_pins_with_shared_ports(void)
{
  stepper_attr_t config;
  uint8_t shared_port;
  uint8_t shared_port_ddr;

  config.dir_port = &shared_port;
  config.dir_port_ddr = &shared_port_ddr;
  config.dir_pin = 0;

  config.enable_port = &shared_port;
  config.enable_port_ddr = &shared_port_ddr;
  config.enable_pin = 1;

  config.step_port = &shared_port;
  config.step_port_ddr = &shared_port_ddr;
  config.step_pin = 2;

  config.ms1_port = &shared_port;
  config.ms1_port_ddr = &shared_port_ddr;
  config.ms1_pin = 3;

  config.ms2_port = &shared_port;
  config.ms2_port_ddr = &shared_port_ddr;
  config.ms2_pin = 4;

  config.ms3_port = &shared_port;
  config.ms3_port_ddr = &shared_port_ddr;
  config.ms3_pin = 5;

  stepper_construct(config, &stepper_handles[0]);

  TEST_ASSERT(shared_port_ddr == 63);
  TEST_ASSERT(shared_port == 2);
}

void test_construct_returns_descriptor_thru_ptr(void)
{
  stepper_handles[0] = 1;
  _makeStepper(0);

  TEST_ASSERT(stepper_handles[0] == 0);
}

void test_construct_returns_success_if_steppers_available(void)
{
  TEST_ASSERT(_makeStepper(0) == STEPPER_ERR_NONE);
}

void test_construct_returns_err_if_none_available(void)
{
  _makeStepper(0);
  _makeStepper(0);
  TEST_ASSERT(_makeStepper(0) == STEPPER_ERR_NONE_AVAILABLE);
}

void test_enable_sets_enable_pin_low(void)
{
  uint8_t handle_index = 0;
  _makeStepper(handle_index);

  stepper_enable(stepper_handles[handle_index]);

  TEST_ASSERT((enable_port & (1 << enable_pin)) == 0);
}

void test_enable_returns_error_when_handle_invalid(void)
{
  uint8_t handle_index = 0;
  uint8_t invalid_handle = 3;
  _makeStepper(handle_index);

  stepper_enable(stepper_handles[handle_index]);

  TEST_ASSERT(
    stepper_enable(invalid_handle)
    == STEPPER_ERR_HANDLE_INVALID
  );
}

void test_disable_sets_enable_pin_high(void)
{
  uint8_t handle_index = 0;
  _makeStepper(handle_index);

  stepper_enable(stepper_handles[handle_index]);
  stepper_disable(stepper_handles[handle_index]);

  TEST_ASSERT(enable_port & (1 << enable_pin));
}

void test_disable_returns_error_when_handle_invalid(void)
{
  uint8_t handle_index = 0;
  uint8_t invalid_handle = 3;
  _makeStepper(handle_index);

  stepper_disable(stepper_handles[handle_index]);

  TEST_ASSERT(
    stepper_disable(invalid_handle)
    == STEPPER_ERR_HANDLE_INVALID
  );
}

void test_set_speed_sets_speed(void)
{
  uint8_t speed = (uint8_t)rand();
  uint8_t handle_index = 0;
  _makeStepper(handle_index);
  stepper_setSpeed(stepper_handles[handle_index], speed);
  TEST_ASSERT(stepper_getSpeed(stepper_handles[handle_index]) == speed);
}

void test_set_speed_throws_error_when_handle_invalid(void)
{
  uint8_t speed = (uint8_t)rand();
  uint8_t handle_index = 0;
  uint8_t invalid_handle = 3;

  _makeStepper(handle_index);
  TEST_ASSERT(
    stepper_setSpeed(invalid_handle, speed)
    == STEPPER_ERR_HANDLE_INVALID
  );
}

void test_setStepSize_sets_step_size(void)
{
  uint8_t i;
  uint8_t step_size;
  uint8_t handle_index = 0;

  _makeStepper(handle_index);

  stepper_setStepSize(stepper_handles[handle_index], STEPPER_STEP_SIZE_FULL);
  TEST_ASSERT((ms1_port & (1 << ms1_pin)) == 0);
  TEST_ASSERT((ms2_port & (1 << ms2_pin)) == 0);
  TEST_ASSERT((ms3_port & (1 << ms3_pin)) == 0);
  stepper_setStepSize(stepper_handles[handle_index], STEPPER_STEP_SIZE_HALF);
  TEST_ASSERT(ms1_port & (1 << ms1_pin));
  TEST_ASSERT((ms2_port & (1 << ms2_pin)) == 0);
  TEST_ASSERT((ms3_port & (1 << ms3_pin)) == 0);
  stepper_setStepSize(stepper_handles[handle_index], STEPPER_STEP_SIZE_QUARTER);
  TEST_ASSERT((ms1_port & (1 << ms1_pin)) == 0);
  TEST_ASSERT(ms2_port & (1 << ms2_pin));
  TEST_ASSERT((ms3_port & (1 << ms3_pin)) == 0);
  stepper_setStepSize(stepper_handles[handle_index], STEPPER_STEP_SIZE_EIGHTH);
  TEST_ASSERT(ms1_port & (1 << ms1_pin));
  TEST_ASSERT(ms2_port & (1 << ms2_pin));
  TEST_ASSERT((ms3_port & (1 << ms3_pin)) == 0);
  stepper_setStepSize(
    stepper_handles[handle_index],
    STEPPER_STEP_SIZE_SIXTEENTH
  );
  TEST_ASSERT(ms1_port & (1 << ms1_pin));
  TEST_ASSERT(ms2_port & (1 << ms2_pin));
  TEST_ASSERT(ms3_port & (1 << ms3_pin));
}

void test_setStepSize_throws_error_when_handle_invalid(void)
{
  uint8_t step_size = STEPPER_STEP_SIZE_FULL;
  uint8_t handle_index = 0;
  uint8_t invalid_handle = 3;

  _makeStepper(handle_index);
  TEST_ASSERT(
    stepper_setStepSize(invalid_handle, step_size)
    == STEPPER_ERR_HANDLE_INVALID
  );
}

void test_setPos_sets_desired_position(void)
{
  uint8_t position = (uint8_t)(rand() % 199);
  uint8_t handle_index = 0;
  _makeStepper(handle_index);

  TEST_ASSERT(
    stepper_setPos(stepper_handles[handle_index], position)
    == STEPPER_ERR_NONE
  );

  TEST_ASSERT(
    stepper_getDesiredPos(stepper_handles[handle_index])
    == position
  );
}

void test_setPos_returns_error_when_handle_invalid(void)
{
  uint8_t position = (uint8_t)(rand() % MAX_STEPPER_POS);
  uint8_t handle_index = 0;
  uint8_t invalid_handle = 3;
  _makeStepper(handle_index);

  stepper_setPos(stepper_handles[handle_index], position);

  TEST_ASSERT(
    stepper_setPos(invalid_handle, position)
    == STEPPER_ERR_HANDLE_INVALID
  );
}

void test_setPos_returns_error_when_position_invalid(void)
{
  uint8_t position = MAX_STEPPER_POS + 1;
  uint8_t handle_index = 0;
  _makeStepper(handle_index);

  TEST_ASSERT(
    stepper_setPos(stepper_handles[handle_index], position)
    == STEPPER_ERR_POSITION_INVALID
  );
}

void test_setDir_sets_direction_bit(void)
{
  stepper_dir_t dir = STEPPER_DIR_FORWARD;
  uint8_t handle_index = 0;
  _makeStepper(handle_index);

  stepper_setDir(stepper_handles[handle_index], dir);
  TEST_ASSERT(stepper_getDir(stepper_handles[handle_index]) == dir);
  TEST_ASSERT((dir_port & (1 << dir_pin)) == 0);
}

void test_set_dir_clears_direction_bit(void)
{
  stepper_dir_t dir = STEPPER_DIR_REVERSE;
  uint8_t handle_index = 0;
  _makeStepper(handle_index);

  stepper_setDir(stepper_handles[handle_index], dir);
  TEST_ASSERT(stepper_getDir(stepper_handles[handle_index]) == dir);
  TEST_ASSERT(dir_port & (1 << dir_pin));
}

void test_setDir_returns_error_when_handle_invalid(void)
{
  stepper_dir_t dir = (stepper_dir_t)(rand() % (NUM_DIRECTIONS-1));
  uint8_t handle_index = 0;
  uint8_t invalid_handle = 3;
  _makeStepper(handle_index);

  stepper_setDir(stepper_handles[handle_index], dir);

  TEST_ASSERT(
    stepper_setDir(invalid_handle, dir)
    == STEPPER_ERR_HANDLE_INVALID
  );
}

void test_stepEngage_sets_step_pin_high(void)
{
  uint8_t handle_index = 0;
  _makeStepper(handle_index);

  stepper_stepEngage(stepper_handles[handle_index]);

  TEST_ASSERT(step_port & (1 << step_pin));
}

void test_stepEngage_returns_error_when_handle_invalid(void)
{
  uint8_t handle_index = 0;
  uint8_t invalid_handle = 3;
  _makeStepper(handle_index);

  stepper_stepEngage(stepper_handles[handle_index]);

  TEST_ASSERT(
    stepper_stepEngage(invalid_handle)
    == STEPPER_ERR_HANDLE_INVALID
  );
}

/*******************************************************************************
* Private Function Definitions
*******************************************************************************/
stepper_err_t _makeStepper(uint8_t handle_index) {
  stepper_attr_t config;
  config.dir_port = &dir_port;
  config.dir_port_ddr = &dir_port_ddr;
  config.dir_pin = dir_pin;

  config.enable_port = &enable_port;
  config.enable_port_ddr = &enable_port_ddr;
  config.enable_pin = enable_pin;

  config.step_port = &step_port;
  config.step_port_ddr = &step_port_ddr;
  config.step_pin = step_pin;

  config.ms1_port = &ms1_port;
  config.ms1_port_ddr = &ms1_port_ddr;
  config.ms1_pin = ms1_pin;

  config.ms2_port = &ms2_port;
  config.ms2_port_ddr = &ms2_port_ddr;
  config.ms2_pin = ms2_pin;

  config.ms3_port = &ms3_port;
  config.ms3_port_ddr = &ms3_port_ddr;
  config.ms3_pin = ms3_pin;

  return stepper_construct(config, &stepper_handles[handle_index]);
}
