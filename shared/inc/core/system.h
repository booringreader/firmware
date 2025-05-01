#ifndef INC_SYSTEM_H
#define INC_SYSTEM_H

#include <common-defines.c>

#define CPU_FREQUENCY       (84000000) //! four cycles for each count increment, hence div by 4
#define SYSTICK_FREQUENCY   (1000) 

void system_setup(void);
uint64_t system_get_ticks(void);

#endif // INC_SYSTEM_H
