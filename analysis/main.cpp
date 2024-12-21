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

    // constexpr double T_c = 2.269;

    double l = 0.1;
    double r = 4;
    double N = 20;
    double J = -1;

    int w = 30;
    int h = 30;

    std::vector<double> energy(N);
    std::vector<double> energy_(N);
    std::vector<double> energy_derivative(N);

    double T = r;
    double dT = (r - l) / double(N - 1);
    // std::cout << "Tc: " << T_c << std::endl;

    std::cout << "T: " << T << ", dT: " << dT << std::endl;  

    T += dT; // T(0) = T

    izing_model::model m;
    m.set_initial_conditions(w,h);
    m.set_only_neighbours(false);
    m.J = J;
    
    int mks = w * h;

    int init_steps = 500;
    int mean_steps = 500;


// from r to l
    for (auto i = 0; i < N; ++i)
    {        
        T -= dT;
        std::cout << i + 1 <<'/'<< N <<std::endl;
        std::cout << "Temperature: " << T << std::endl;

        m.temperature = T;

        m.process(mks * init_steps);

        std::vector<double> energy_values(mean_steps);
        for (auto i = 0; i < mean_steps; ++i)
        {
            m.process(mks);
            energy_values[i] = m.get_full_energy();
        }

        auto average_energy = std::accumulate(energy_values.begin(),energy_values.end(),0.) / double(mean_steps) / w / h;
        energy[i] = average_energy;
    }

    T -= dT;
//     from l to r
    for (auto i = 0; i < N; ++i)
    {
        T += dT;
        std::cout << i + 1 <<'/'<< N <<std::endl;
        std::cout << "Temperature: " << T << std::endl;

        m.temperature = T;

        m.process(mks * init_steps);

        std::vector<double> energy_values(mean_steps);
        for (auto i = 0; i < mean_steps; ++i)
        {
            m.process(mks);
            energy_values[i] = m.get_full_energy();
        }

        auto average_energy = std::accumulate(energy_values.begin(),energy_values.end(),0.) / double(mean_steps) / w / h;
        energy_[i] = average_energy;
    }

    for (auto i = 0 ; i <energy.size(); ++i)
    {
        energy[i] += *(energy_.rbegin() + i);
        energy[i] /= 2;
    }


    // std::reverse(energy.begin(),energy.end());
    differentiate(energy.rbegin(), energy.rend(), energy_derivative.rbegin());
    // std::reverse(energy.begin(),energy.end());


    T = r;
    T += dT; // T(0) = T

    for (auto i = 0; i < N; ++i)
    {
        T -= dT;

        file_energy << T << ' ' << energy[i] << std::endl;
        file_energy_derivative << T << ' ' << energy_derivative[i] << std::endl;
    }

    return 0;
}
