#include <zephyr/devicetree.h>

#define USERROW_NODE DT_NODELABEL(userrow)

    .userpage DT_REG_ADDR(USERROW_NODE) :
    {
        KEEP(*(.userpage))
    }
