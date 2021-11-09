#include <iostream>

#include "InfINT.h"

int main()
{
    InfINT arr(1, 1024);
    arr.factor(100);
    std::cout << arr.getString();
    return 0;
}
