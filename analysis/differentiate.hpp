#include <assert.h>

template <typename InputIt, typename OutputIt, typename Real = double>
void differentiate(InputIt begin, InputIt end, OutputIt output, Real step = 1.0)
{
    auto n = end - begin;
    assert(n > 2);

    *(output + 0) = -3* *(begin + 0) + 4 * *(begin + 1) - *(begin + 2);
    *(output + 0) /= 2 * step;

    for (auto i = 1; i < n - 1; ++i)
    {
        *(output + i) = (*(begin + i + 1) - *(begin + i - 1)) / 2.0 / step;
    }

    *(output + n - 1) = *(end - 1 - 2) - 4* *(end - 1 - 1) + 3* *(end - 1);
    *(output + n - 1) /= 2 * step;
}