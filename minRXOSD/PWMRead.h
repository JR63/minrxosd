/**
 ******************************************************************************
 *
 * @file       PWMRead.h
 * @author     Joerg-D. Rothfuchs
 * @brief      Implements a non int version of PWM read (do int version later)
 *
 *****************************************************************************/


#ifndef PWMREAD_H_
#define PWMREAD_H_

// TODO:
// implement int version later

#define PWM_IN_PIN		3		// minimOSD PAL pin
#define PWM_TIMEOUT		25000		// micro seconds

#define PWM_CHECK(x)		(x > 1700)

void pwm_read_init(void);
void pwm_read(void);
int pwm_get(void);

#endif /* PWMREAD_H_ */