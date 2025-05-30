#include "../inc/common-defines.c"
#include <libopencm3/stm32/memorymap.h>
#include <libopencm3/cm3/vector.h>

#define BOOTLOADER_SIZE         (0x8000U)                           // 32KB (U is for unsigned valeu)
#define MAIN_APP_START_ADDRESS  (FLASH_BASE + BOOTLOADER_SIZE)      // FLASH_BASE variable makes it slightly more hardware independent

static void jump_to_main(void){
    vector_table_t* vector_table = (vector_table_t*)(MAIN_APP_START_ADDRESS); 
    main_vector_table->reset();
}

int main(void){
    jump_to_main();

    //! never return, control is transferred to firmware from jump_to_main()
    return 0;
}