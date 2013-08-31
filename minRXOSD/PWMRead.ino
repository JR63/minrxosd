/**
 ******************************************************************************
 *
 * @file       PWMRead.ino
 * @author     Joerg-D. Rothfuchs
 * @brief      Implements a non int version of PWM read (do int version later)
 *
 *****************************************************************************/


#include "PWMRead.h"

static int pwm_duration;

void pwm_read_init(void)
{
	pinMode(PWM_IN_PIN, INPUT);
}

void pwm_read(void)
{
	pwm_duration = (int) pulseIn(PWM_IN_PIN, HIGH, PWM_TIMEOUT);
}

int pwm_get(void)
{
	return pwm_duration;
}