#include <iostream>

#include <array>
#include <string>
#include <format>
#include "InfINT.h"
#include "Utility.h"
#include "Radix.h"
#include <bitset>

int main()
{
	auto [hi, lo] = infini::util::half(3489579857938475439ull);
	auto bigint = infini::Infinint(3489579857938475439ull);

	std::cout << 3489579857938475439ull << '\n';
	std::cout << infini::to_string(bigint);

	return 0;
}
