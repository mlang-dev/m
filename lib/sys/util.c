#include <stdbool.h>
#include <stdint.h>

bool is_power_of2_64(uint64_t value)
{
    return value && !(value & (value - 1));
}
