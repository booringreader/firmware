#include "../inc/common-defines.c"
#include <libopencm3/stm32/memorymap.h>

#define BOOTLOADER_SIZE         (0x8000U)                           // 32KB (U is for unsigned valeu)
#define MAIN_APP_START_ADDRESS  (FLASH_BASE + BOOTLOADER_SIZE)      // FLASH_BASE variable makes it slightly more hardware independent

static void jump_to_main(void){
    typedef void (*void_fn)(void);

    uint32_t* reset_vector_entry = (uint32_t*)(MAIN_APP_START_ADDRESS + 4U); // codeblock address is stored at the second entry of the IVT; entry of the IVT
    uint32_t* reset_vector = (uint32_t*)(*reset_vector_entry); // value stored at the addresss stored in reset_vector_entry is also an address, address of the code block

    // convert the address to a function pointer
    void_fn jump_fn = (void_fn)reset_vector;
    jump_fn();
}

int main(void){
    jump_to_main();

    //! never return, control is transferred to firmware from jump_to_main()
    return 0;
}