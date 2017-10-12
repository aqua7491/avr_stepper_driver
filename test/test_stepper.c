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
  dir_pin = (1 << (rand() % 8 + 0));

  enable_port = (uint8_t)rand();
  enable_port_ddr = 0;
  enable_pin = (1 << (rand() % 8 + 0));

  step_port = (uint8_t)rand();
  step_port_ddr = 0;
  step_pin = (1 << (rand() % 8 + 0));

  ms1_port = (uint8_t)rand();
  ms1_port_ddr = 0;
  ms1_pin = (1 << (rand() % 8 + 0));

  ms2_port = (uint8_t)rand();
  ms2_port_ddr = 0;
  ms2_pin = (1 << (rand() % 8 + 0));

  ms3_port = (uint8_t)rand();
  ms3_port_ddr = 0;
  ms3_pin = (1 << (rand() % 8 + 0));
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

  TEST_ASSERT(dir_port == 0);
  TEST_ASSERT(dir_port_ddr == dir_pin);

  TEST_ASSERT(enable_port == 0);
  TEST_ASSERT(enable_port_ddr == enable_pin);

  TEST_ASSERT(step_port == 0);
  TEST_ASSERT(step_port_ddr == step_pin);

  TEST_ASSERT(ms1_port == 0);
  TEST_ASSERT(ms1_port_ddr == ms1_pin);

  TEST_ASSERT(ms2_port == 0);
  TEST_ASSERT(ms2_port_ddr == ms2_pin);

  TEST_ASSERT(ms3_port == 0);
  TEST_ASSERT(ms3_port_ddr == ms3_pin);
}

void test_construct_initializes_hardware_pins_with_shared_ports(void)
{
  stepper_attr_t config;
  uint8_t shared_port;
  uint8_t shared_port_ddr;

  config.dir_port = &shared_port;
  config.dir_port_ddr = &shared_port_ddr;
  config.dir_pin = 1;

  config.enable_port = &shared_port;
  config.enable_port_ddr = &shared_port_ddr;
  config.enable_pin = 2;

  config.step_port = &shared_port;
  config.step_port_ddr = &shared_port_ddr;
  config.step_pin = 4;

  config.ms1_port = &shared_port;
  config.ms1_port_ddr = &shared_port_ddr;
  config.ms1_pin = 8;

  config.ms2_port = &shared_port;
  config.ms2_port_ddr = &shared_port_ddr;
  config.ms2_pin = 16;

  config.ms3_port = &shared_port;
  config.ms3_port_ddr = &shared_port_ddr;
  config.ms3_pin = 32;

  stepper_construct(config, &stepper_handles[0]);

  TEST_ASSERT(shared_port_ddr == 63);
  TEST_ASSERT(shared_port == 0);
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
  for (i=0;i<NUM_STEP_SIZE_OPTIONS;i++) {

  }

  stepper_setStepSize(stepper_handles[handle_index], STEPPER_STEP_SIZE_FULL);
  TEST_ASSERT(ms1_port == 0);
  TEST_ASSERT(ms2_port == 0);
  TEST_ASSERT(ms3_port == 0);
  stepper_setStepSize(stepper_handles[handle_index], STEPPER_STEP_SIZE_HALF);
  TEST_ASSERT(ms1_port == ms1_pin);
  TEST_ASSERT(ms2_port == 0);
  TEST_ASSERT(ms3_port == 0);
  stepper_setStepSize(stepper_handles[handle_index], STEPPER_STEP_SIZE_QUARTER);
  TEST_ASSERT(ms1_port == 0);
  TEST_ASSERT(ms2_port == ms2_pin);
  TEST_ASSERT(ms3_port == 0);
  stepper_setStepSize(stepper_handles[handle_index], STEPPER_STEP_SIZE_EIGHTH);
  TEST_ASSERT(ms1_port == ms1_pin);
  TEST_ASSERT(ms2_port == ms2_pin);
  TEST_ASSERT(ms3_port == 0);
  stepper_setStepSize(
    stepper_handles[handle_index],
    STEPPER_STEP_SIZE_SIXTEENTH
  );
  TEST_ASSERT(ms1_port == ms1_pin);
  TEST_ASSERT(ms2_port == ms2_pin);
  TEST_ASSERT(ms3_port == ms3_pin);
}

// void test_setStepSize_sets_step_size(void)
// {
//   uint8_t step_size = (stepper_step_size_t)(rand() % 4);
//   uint8_t handle_index = 0;
//
//   _makeStepper(handle_index);
//   stepper_setStepSize(stepper_handles[handle_index], step_size);
//   TEST_ASSERT(stepper_getStepSize(stepper_handles[handle_index]) == step_size);
// }

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
