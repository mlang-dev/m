/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * c math functions
 */
int pow_int(int base, int exp)
{
    if (exp < 0)
        return 0;
    int pow = 1;
    while (exp) {
        if (exp % 2)
            pow *= base;
        base *= base;
        exp /= 2;
    }
    return pow;
}
