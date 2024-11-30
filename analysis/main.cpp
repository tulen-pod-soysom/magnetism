#include "../magnetism/izing_model.hpp"
#include "differentiate.hpp"
#include <algorithm>
#include <fstream>
#include <numeric>
#include <vector>
#include <execution>
#include <algorithm>

int main(int argc, char** argv)
{
    std::ofstream file_energy("energy.txt");
    std::ofstream file_energy_derivative("energy_derivative.txt");


    constexpr double T_c = 2.269;

    double l = 0.3;
    double r = 1.5;
    double N = 100;
    double J = 1;

    int w = 10;
    int h = 10;

    int steps = 100000;
    int averaging_steps = 100'000;

    std::vector<double> energy(N);
    std::vector<double> energy_derivative(N);


    for (auto i = 0; i < N; ++i)

    // std::vector<int>cycles(N);
    // std::iota(cycles.begin(), cycles.end(), 0);
    // std::for_each(std::execution::par,cycles.begin(),cycles.end(),[&] (auto i)
    {
        
        double T = l * T_c + (r-l)*T_c*i/double(N);

        izing_model::model m;

        m.set_initial_conditions(w,h);
        m.J = J;
        m.temperature = T;

        m.process(steps * w * h);

        std::vector<double> energy_values(averaging_steps);
        for (auto i = 0; i < averaging_steps; ++i)
        {
            m.process();
            energy_values[i] = m.get_full_energy();
        }

        auto average_energy = std::accumulate(energy_values.begin(),energy_values.end(),0.) / double(averaging_steps) / double(w*h);
        energy[i] = average_energy;
        std::cout << i <<'/'<< N <<std::endl;
    }
    // );

    differentiate(energy.begin(), energy.end(), energy_derivative.begin());

    for (auto i = 0; i < N; ++i)
    {
        double T = l * T_c + (r-l)*T_c*i/double(N);

        file_energy << T << ' ' << energy[i] << std::endl;
        file_energy_derivative << T << ' ' << energy_derivative[i] << std::endl;
    }

    return 0;
}