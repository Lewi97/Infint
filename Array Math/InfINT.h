#pragma once

#include <vector>
#include "Utility.h"
#include <ranges>
#include <string>
#include <assert.h>
#include <charconv>

namespace infini
{
    /* Forward decl */
    class Infinint;
    inline auto to_infinint(std::string_view view, Infinint& other) -> void;

    class Infinint
    {
    public:
        using Base = uint32_t;
    protected:
        friend auto to_string(const Infinint& infi) -> std::string;

        std::vector<Base> _number;

        constexpr auto 
            resize_add(size_t amount)
        {
            _number.resize(capacity() + amount + 1);
        }

        constexpr auto 
            resize_double()
        {
            _number.resize(capacity() * 2);
        }

        constexpr auto 
            resize_to(size_t amount)
        {
            _number.resize(amount + 1);
        }
    public:
        constexpr 
            Infinint()
            : _number(2, 0)
        {}
        
        constexpr explicit
            Infinint(Base base)
            : _number(2, 0)
        {
            at(0) = base;
        }

        explicit
            Infinint(std::string_view view)
        {
            to_infinint(view, *this);
        }

        constexpr explicit
            Infinint(uint64_t n)
            : _number(4, 0)
        {
            auto [hi, lo] = util::half(n);
            at(0) = lo;
            at(1) = hi;
        }

        /*
        * Most significant byte first
        * [0, 1] == Radix
        * [1, 0] == 1
        */
        constexpr explicit
            Infinint(std::vector<Base> data)
            : _number(data)
        {}

        constexpr auto 
            add(const Infinint& other) -> Infinint&
        {
            if (capacity() < other.size())
            {
                resize_to(other.size() + 2);
            }

            for (auto idx{ 0ull }; auto number : other._number)
            {
                add(number, idx);
                idx++;
            }

            return *this;
        }
        
        constexpr auto
            add(Base amount) -> Infinint&
        {
            if (util::overflow_add(at(0), amount))
            {
                add(Base{ 1 }, 1);
            }

            return *this;
        }

        constexpr auto
            add(uint64_t amount) -> Infinint&
        {
            auto [hi, lo] = util::half(amount);

            add(lo);
            add(hi, 1);

            return *this;
        }

        constexpr auto 
            add(Base amount, size_t position) -> Infinint&
        {
            if (capacity() <= position)
            { /* plus 2 to account for two carry's, better to allocate more */
                resize_to(position + 2);
            }

            if (util::overflow_add(at(position), amount))
            {
                add(Base{ 1 }, position + 1);
            }

            return *this;
        }

        constexpr auto
            divide(Base divisor) -> Infinint&
        {
            constexpr auto pow32 = util::pow(2ull, 32);
            auto remainder{ 0ull };

            for (auto iter = _number.rbegin(); iter != _number.rend(); iter++)
            {
                auto sum = pow32 * remainder + static_cast<uint64_t>(*iter);
                *iter = static_cast<uint32_t>(sum) / divisor;
                remainder = sum % divisor;
            }

            return *this;
        }

        constexpr auto
            multiply(Base multiplier, size_t position = 0) -> Infinint&
        { /* Multiplier cant be uint64_t cause it could overflow the calculation if multiplier was sufficiently high */
            if (multiplier == 1) return *this;
            
            auto carry = uint64_t{ 0 };

            for (auto idx{ position }; idx < size(); idx++)
            {
                auto& num = at(idx);
                auto sum = static_cast<uint64_t>(num) * static_cast<uint64_t>(multiplier) + carry;
                carry = static_cast<Base>(sum >> 32); /* Take high bits */
                num = static_cast<Base>(sum); /* Take low bits */

                if (idx == size() - 1 && carry != 0)
                {
                    resize_double();
                }
            }

            return *this;
        }

        /* (TODO) Handle negative */
        /* (TODO) Handle subtraction */
        /* (TODO) Handle bitshifts */
        /* (TODO) Handle conditionals */

        constexpr auto 
            size() const -> size_t { return _number.size(); }
        
        constexpr auto 
            capacity() const -> size_t { return _number.capacity(); }
        
        constexpr auto 
            at(size_t index) -> Base& { return _number.at(index); }
        
        constexpr auto 
            empty() const -> bool { return _number.empty(); }
        
        static constexpr auto 
            radix() -> uint64_t { return static_cast<uint64_t>(std::numeric_limits<Base>::max()) + 1; }
        
        constexpr auto
            approx_digits(uint64_t size) -> uint64_t { return size * 10 + 1; }
    };
    
    /*
    * Using the naive solution of just div 10, mod 10 till we crafted the end result
    */
    inline auto 
        to_string(const Infinint& infi) -> std::string
    {
        auto string = std::string();
        /* Copy of internal buffer, not sure how to do this without modifying the internal buffer
        so we just have to deal with allocating on string conversions */
        auto buffer_copy = infi._number; 
        const auto end = buffer_copy.rend();
        auto iter = buffer_copy.rbegin();

        while (iter != end && *iter == 0)
            iter++;

        auto digits_left = infi.size() - std::distance(buffer_copy.rbegin(), iter);
        while (digits_left > 0)
        {
            auto remainder = 0ull;

            for (auto i{ 0ull }; i < digits_left; i++)
            {
                constexpr auto pow32 = util::pow(2ull, 32);
                auto sum = remainder * pow32 + iter[i];
                iter[i] = static_cast<uint32_t>(sum / 10ull);
                remainder = sum % 10ull;
            }

            string += static_cast<char>(util::to_ascii_digit(remainder));

            if (*iter == 0)
            {
                iter++;
                digits_left--;
            }
        }

        /* In the future we ought to approximate digits in infi and populate string backwards */
        std::reverse(string.begin(), string.end());

        return string;
    }

    /*
    * Creates an infinint from a string view. 
    * If a non digit character is found in the view the construction will stop.
    */
    inline auto
        to_infinint(std::string_view view) -> Infinint
    {
        auto infinint = Infinint{};

        /* pow64 = 18446744073709551616 -> 20 digits */
        constexpr auto digits_in_pow64 = 20ull;
        /* pow32 = 4294967295 -> 10 digits */
        constexpr auto digits_in_pow32 = 10ull;
        
        auto chunk_size = view.size() <= digits_in_pow64 - 2 /* -2 else we risk overflowing */ 
            ? view.size() : digits_in_pow64 - 2;

        while (not view.empty())
        {
            auto from_chars = uint64_t{};

            /*
            * We cant multiply directly by chunk_size due to uint64_t > uint32_t so we have to half chunk_size 
            * This could be solved by multiplying by another Infinint but that would be a needless heap allocation
            */
            if (chunk_size < digits_in_pow32)
            {
                infinint.multiply(util::pow(10u, chunk_size));
            }
            else
            {
                auto half_chunk_size = chunk_size / 2;

                /* if chunk_size is odd add 1, integer division floors the result */
                infinint.multiply(util::pow(10ul, half_chunk_size + (chunk_size & 1)));
                infinint.multiply(util::pow(10ul, half_chunk_size));
            }


            auto [_, err] = std::from_chars(view.data(), view.data() + chunk_size, from_chars);
            if (err == std::errc::invalid_argument) break;

            infinint.add(from_chars);

            view.remove_prefix(chunk_size);

            if (chunk_size > view.size())
                chunk_size = view.size();
        }

        return infinint;
    }

    inline auto
        to_infinint(std::string_view view, Infinint& other) -> void
    {
        other = to_infinint(view);
    }
}
namespace infini::literals
{
    auto operator"" _infinint(const char* string, size_t size) -> Infinint
    {
        return to_infinint(std::string_view{ string });
    }
}
