#include "unity.h"
#include <time.h>
#include <stdlib.h>
/*******************************************************************************
* Module Under Test
*******************************************************************************/
#include "stepper.h"

/*******************************************************************************
* Local Data
*******************************************************************************/
static uint8_t dir_port;
static uint8_t dir_port_ddr;
static uint8_t dir_pin;
static uint8_t enable_port;
static uint8_t enable_port_ddr;
static uint8_t enable_pin;
static uint8_t step_port;
static uint8_t step_port_ddr;
static uint8_t step_pin;

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
}

void tearDown(void)
{
}

/*******************************************************************************
* Tests
*******************************************************************************/
void test_construct_initializes_hardware_pins_with_individual_ports(void)
{
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

  stepper_construct(config);

  TEST_ASSERT(dir_port == 0);
  TEST_ASSERT(dir_port_ddr == dir_pin);

  TEST_ASSERT(enable_port == 0);
  TEST_ASSERT(enable_port_ddr == enable_pin);

  TEST_ASSERT(step_port == 0);
  TEST_ASSERT(step_port_ddr == step_pin);
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

  stepper_construct(config);

  TEST_ASSERT(shared_port_ddr == 7);
  TEST_ASSERT(shared_port == 0);
}
