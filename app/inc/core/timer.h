#ifndef INC_TIMER
#define INC_TIMER

void timer_setup(void);
void timer_pwm_set_duty_cycle(float duty_cycle); // take an argument b/w 0 and 100, for 0% to 100%

#endif