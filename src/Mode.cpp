#include <Mode.h>

uint32_t Mode::toHex(String hex)
{
    long a = strtol(hex.substring(1).c_str(), 0, 16);
    return (uint32_t)a;
}