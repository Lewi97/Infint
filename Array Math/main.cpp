#include <iostream>

#include <array>
#include <string>
#include <format>
#include "InfINT.h"
#include "Utility.h"
#include "Radix.h"
#include <bitset>

auto factorial(uint64_t n) -> infini::Infinint
{
	auto out = infini::Infinint(1u);
	while (n != 1)
	{
		out.multiply(n--);
	}
	return out;
}

using namespace infini::literals;

int main()
{
	auto bigint = "2000000000000000000"_infinint;

	std::cout << infini::to_string(bigint) << '\n';

	return 0;
}
