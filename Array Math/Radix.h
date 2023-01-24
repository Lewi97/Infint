#pragma once

#include <concepts>
#include <string>
#include <string_view>
#include <format>
#include <math.h>

/*
* Test code to mess around with learning about converting from and to different bases.
*/

namespace util::rad
{
	template<std::integral _Int>
	constexpr inline auto 
		mod_div(_Int& number, _Int by)
	{
		auto mod = number % by;
		number /= by;
		return mod;
	}

	constexpr inline auto
		pow(std::unsigned_integral auto number, uint64_t p) -> decltype(number)
	{
		if (p == 0) return decltype(number){1};
		auto sum = static_cast<decltype(number)>(number);
		for (auto i{ 1ull }; i < p; i++)
			sum *= number;
		return sum;
	}

	/*
	* @param n: Number in base10
	* @param alphabet: The tokens to use in representing the number in its new base.
	* The radix derived from the alphabets size.
	* 
	* @Formula for base10 645 to base8
		645 / 8 = floor(80,625) = 80 * 8 = 640 + 5  = 645
		80  / 8 = floor(10)     = 10 * 8 = 80  + 0  = 80
		10  / 8 = floor(1,25)   = 1  * 8 = 8   + 2  = 10
		left:                                  + 1
	*/
	constexpr inline auto
		to_base(std::integral auto base10_num, std::string_view alphabet) -> std::string
	{
		if (alphabet.empty()) return std::string{};
		if (base10_num == 0) return std::string{ alphabet[0] };

		const auto radix = alphabet.size();
		const auto negative = base10_num < 0;
		auto string = std::string{};

		auto sum = static_cast<uint64_t>(std::abs(base10_num));

		while (sum > (radix - 1))
		{
			auto result = sum / radix;
			auto token = sum - result * radix;
			string += alphabet[token];
			sum = result;
		}

		if (sum != 0)
			string += alphabet[sum];

		if (negative)
			string += '-';

		std::reverse(string.begin(), string.end());

		return string;
	}

	/* 321(3) =
				3 * 3^2 +
				2 * 3^1 +
				1 * 3^0 */
	constexpr inline auto
		to_base10(std::integral auto number, uint64_t radix) -> decltype(number)
	{
		auto sum = int64_t{};
		auto factor = 0;
		
		while (number > 0)
		{ 
			auto remainder = mod_div(number, 10);
			
			sum += remainder * pow(radix, factor);

			factor += 1;
		}

		return sum;
	}
}

