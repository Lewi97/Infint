#pragma once

#include <string>

class InfINT {
private:
    int* a = nullptr;
    size_t size = 0UL;
    const size_t max = 64UL;
public:
    InfINT(int num, size_t max = 64UL)
        : max(max)
    {
        a = new int[max];
        cleanse();
        size = numLength(num);
        addition(num);
    }

    int numLength(int num)
    {
        int n = 0;
        while (num > 0) { n++; num /= 10; }
        return n;
    }

    void updateSize()
    {
        size_t i = 0;
        while (a[i] == 0) i++;
        size = max - i;
    }

    std::string getString()
    {
        std::string num;
        for (size_t i = max - size; i < max; i++)
        {
            std::string n = std::to_string(a[i]);
            num.append(n);
        }
        return num;
    }

    void addition(int toAdd)
    {
        int i = max;
        while (toAdd > 0 && i-- > 0)
        {
            a[i] += toAdd % 10;
            toAdd /= 10;
            if (a[i] < 10) continue;
            a[i] -= 10;
            a[i - 1] += 1;
        }
        updateSize();
    }

    void multiplication(int times)
    {
        for (size_t i = max - 1; i >= max - size; i--)
            a[i] *= times;
        for (size_t i = max - 1; i >= max - size; i--)
        {
            if (a[i] < 10) continue;
            handleCarry(i);
        }

        updateSize();
    }

    void handleCarry(size_t i)
    {
        if (a[i] < 10) return;
        int toKeep = a[i] % 10;
        int toCarry = (a[i] - toKeep) / 10;
        a[i - 1] += toCarry;
        a[i] = toKeep;
        handleCarry(i - 1);
    }

    void cleanse()
    {
        for (int i = 0; i < max; i++) a[i] = 0;
    }

    void factor(int to)
    {
        cleanse();
        addition(to);
        for (int i = to - 1; i > 1; i--)
            multiplication(i);
        updateSize();
    }
};

