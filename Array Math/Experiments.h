#pragma once

#include <vector>
#include <string>
#include "Utility.h"
#include "Radix.h"

/*
* @param dec: decimal char array, output
* @param n: size of array b32
* @param b32: big num array with radix 2^32
*/
inline auto
my_convert(std::vector<uint32_t> b32) -> std::string
{
    auto string = std::string( /*approx_infinint32_digits(b32.size()), '0'*/);
    auto iter = b32.begin();

    while (iter != b32.end() && *iter == 0)
        iter++;

    auto digits_left = b32.size() - std::distance(b32.begin(), iter);
    while (digits_left > 0)
    {
        auto remainder = 0ull;

        for (auto i{ 0ull }; i < digits_left; i++)
        {
            constexpr auto pow32 = rad::pow(2ull, 32);
            auto sum = remainder * pow32 + iter[i];
            iter[i] = static_cast<uint32_t>(sum / 10ull);
            remainder = sum % 10ull;
        }

        string += static_cast<char>(infini::util::to_ascii_digit(remainder));

        if (*iter == 0)
        {
            iter++;
            digits_left--;
        }
    }

    std::ranges::reverse(string);

    return string;
}
