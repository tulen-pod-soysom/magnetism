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
    double N = 20;
    double J = 1;

    int w = 100;
    int h = 100;

    std::vector<double> energy(N);
    std::vector<double> energy_derivative(N);

    double T = r * T_c;
    double dT = (r - l) * T_c / double(N);
    std::cout << "Tc: " << T_c << std::endl;

    std::cout << "T: " << T << ", dT: " << dT << std::endl;  

    T += dT; // T(0) = T

    izing_model::model m;
    m.set_initial_conditions(w,h);
    m.set_only_neighbours(false);
    m.J = J;
    
    int mks = w * h;

    int init_steps = 100;
    int mean_steps = 100;

    for (auto i = 0; i < N; ++i)
    {        
        T -= dT;
        std::cout << "Temprature: " << T << std::endl;

        m.temperature = T;

        m.process(mks * init_steps);

        std::vector<double> energy_values(mean_steps);
        for (auto i = 0; i < mean_steps; ++i)
        {
            m.process(mks);
            energy_values[i] = m.get_full_energy();
        }

        auto average_energy = std::accumulate(energy_values.begin(),energy_values.end(),0.) / double(mean_steps);
        energy[i] = average_energy;
        std::cout << i + 1 <<'/'<< N <<std::endl;
    }

    // std::reverse(energy.begin(),energy.end());
    differentiate(energy.rbegin(), energy.rend(), energy_derivative.begin());
    // std::reverse(energy.begin(),energy.end());


    T = r * T_c;
    T += dT; // T(0) = T

    for (auto i = 0; i < N; ++i)
    {
        T -= dT;

        file_energy << T << ' ' << energy[i] << std::endl;
        file_energy_derivative << T << ' ' << energy_derivative[i] << std::endl;
    }

    return 0;
}
