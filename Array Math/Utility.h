#pragma once

#include <concepts>
#include <limits>

namespace infini::concepts
{
	template<typename _Type, typename _Other>
	concept can_stream_in = requires(_Type & type, _Other other)
	{
		{ type << other };
	};
}

namespace infini::util
{
	/*
	* @brief Checks if a + b results in an overflow
	*/
	template<std::unsigned_integral _Int>
	constexpr inline auto 
		overflows(_Int a, _Int b) -> bool
	{
		return a > (std::numeric_limits<_Int>::max() - b);
	}

	/*
	* @brief Checks if a - b results in an underflow
	*/
	template<std::unsigned_integral _Int>
	constexpr inline auto
		underflows(_Int a, _Int b) -> bool
	{
		return b > a;
	}

	/*
	* Adds rhs to lhs and returns wether the addition resulted in an overflow
	* Unsigned overflow is defined, the remainder will be saved in lhs
	* The remainder is always lesser than rhs, except if there was no overflow
	*/
	template<std::unsigned_integral _Int>
	constexpr inline auto
		overflow_add(_Int& lhs, _Int rhs) -> bool
	{
		lhs += rhs;
		return lhs < rhs;
	}

	/*
	* Raises number to power p. std::pow isn't constexpr.
	*/
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
	* Will return the char variant of digit n.
	* Will clamp digit n between '0' and '9' if n is too big to represent with a char.
	*/
	constexpr inline auto
		to_ascii_digit(std::integral auto n) -> char
	{
		auto out = '0' + n;

		return out >= '0' && out <= '9' ? out : '0';
	}
	
	/*
	* Writes the provided range to parameter to using the shift left stream operator.
	* @param range: A range type.
	* @param to: A type whom's left shift operator can be overloaded with the value type of param range.
	*/
	template<std::ranges::range _Range>
	inline auto 
		write(const _Range& range, concepts::can_stream_in<typename _Range::value_type> auto& to)
	{
		to << '[';

		auto i{ range.cbegin() };
		for (; i != range.end() - 1; i++)
			to << *i << ", ";

		to << *i << ']';
	}

	/*
	* Get the high 32 and low 32 bits of a 64 bit unsigned integral.
	* @returns pair{ hi bits, lo bits }
	*/
	constexpr inline auto
		half(uint64_t num) -> std::pair<uint32_t, uint32_t>
	{
		auto lo = static_cast<uint32_t>(num);
		auto hi = static_cast<uint32_t>(num >> 32);
		return std::pair(hi, lo);
	}
}
