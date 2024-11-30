#ifndef UTILS_H
#define UTILS_H

#include <cstddef>
#include <numeric>
#include <vector>

template <typename T>
class moving_average
{
    size_t n;

    std::vector<T> values;
    T average_sum{};

public:
    auto get_average()
    {
        // return average_sum / double(n);
        return std::accumulate(values.begin(),values.end(),0) / double(n);
    }

    void push_back(T a)
    {
        // average_sum += a - values[0];

        for (auto i = 1; i < n - 1; ++i)
            values[i-1] = values[i];

        *values.rbegin() = a;
    }

    moving_average(size_t n = 16) : n(n), values(n,0.0), average_sum(0) {}
};



#endif // UTILS_H
