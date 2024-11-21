#ifndef IZING_MODEL_H
#define IZING_MODEL_H

#include <armadillo>
#include <cstdint>
#include <random>

namespace izing_model {

using namespace arma;

static std::random_device rd;

struct model {
  Mat<char> spins;

  double J = 1;
  constexpr static double m_boltzman = 1.38E-23;
  double temperature =0 ;
  double energy = 0;
  int w,h;


//  0######0
//  #pmppmm#
//  #pmmpmm#
//  #ppppmm#
//  #pmpppm#
//  #mmpmmp#
//  #pmppmm#
//  0######0
//
//   where # is equal to the last known element on the opposite side
  auto set_initial_conditions(int w, int h)
  {
    this->w = w;
    this->h = h;
    spins = Mat<char>(w+2,h+2);
    
    std::uniform_int_distribution<int> dist(0,1);

    for (auto i = 1; i < w+1; ++i)
      for (auto j = 1; j < h+1; ++j)
      {
          auto a = (dist(rd) == 1)? -1:1;
          spins(i,j) = a;
      }

    use_periodic_boundary();
    energy = get_full_energy();
  }

  void use_periodic_boundary()
  {
    for (auto i = 1; i < w+1; ++i)
    {
      spins(i,h+1) = spins(i,1);
      spins(i,0) = spins(i,h);
    }

    for (auto j = 1; j < h+1; ++j)
    {
      spins(w+1,j) = spins(1,j);
      spins(0,j) = spins(w,j);
    }

    spins(0  ,0  ) = 0;
    spins(w+1,0  ) = 0;
    spins(0  ,h+1) = 0;
    spins(w+1,h+1) = 0;

  }

  template <typename Function>
  auto for_each_spin(Function f)
  {
    for (auto i = 1; i < w+1; ++i)
      for (auto j = 1; j < h+1; ++j)
        f(spins(i,j));
  }

  int get_summary_spin()
  {
    int spin = 0;
    for_each_spin([&](auto& a){spin += a;});
    return spin;
  }

  double get_full_energy()
  {
    double energy = 0;

    for (auto i = 1; i < w+1; ++i)
      for (auto j = 1; j < h+1; ++j)
      {
        energy += spins(i,j) * spins(i-1,j);
        energy += spins(i,j) * spins(i,j-1);
      }

    return -J * energy;
  }


  auto delta_energy(int i1,int j1, int i2, int j2)
  {
      double energy = 0;
      if ((abs(i1 - i2) > 1) || (abs(j1 - j2) > 1))
      {
          auto s1 = -spins(i1,j1);
          auto s2 = -spins(i2,j2);
          energy += s1 * (spins(i1-1,j1) + spins(i1+1,j1) + spins(i1,j1-1) + spins(i1,j1+1));
          energy += s2 * (spins(i2-1,j2) + spins(i2+1,j2) + spins(i2,j2-1) + spins(i2,j2+1));
      }
      return energy;
  }

//   using kawasaki dynamic
  auto monte_carlo_step(bool only_neighbours = true)
  {
    std::uniform_int_distribution<int> dist_w(1,w);
    std::uniform_int_distribution<int> dist_h(1,h);

init_first_pair:

    auto i1 = dist_w(rd);
    auto j1 = dist_h(rd);

    if (only_neighbours)
    {
      auto& s0 = spins(i1,j1);

      auto& s1 = spins(i1+1,j1+0);
      auto& s2 = spins(i1-1,j1+0);
      auto& s3 = spins(i1+0,j1+1);
      auto& s4 = spins(i1+0,j1-1);

      std::vector<uint8_t> v;

      if (s0 != s1) {v.push_back(1);}
      if (s0 != s2) {v.push_back(2);}
      if (s0 != s3) {v.push_back(3);}
      if (s0 != s4) {v.push_back(4);}

      if (v.empty()) goto init_first_pair;

      std::uniform_int_distribution<int> dist(0,v.size()-1);

      int i = v[dist(rd)];

      double de = 0;

      switch(i)
      {
      case 1: de = delta_energy(i1,j1,i1+1,j1+0); break;
      case 2: de = delta_energy(i1,j1,i1-1,j1+0); break;
      case 3: de = delta_energy(i1,j1,i1+0,j1+1); break;
      case 4: de = delta_energy(i1,j1,i1+0,j1-1); break;
      }

      if (metropolis_algorithm(de))
      {

      s0 = -s0;

      switch (i) {
        case 1: s1 = -s1; break;
        case 2: s2 = -s2; break;
        case 3: s3 = -s3; break;
        case 4: s4 = -s4; break;
      }
      }


    }
    else
    {
      throw; // todo later
    }
  }

bool metropolis_algorithm(double delta_energy)
      {
            static std::random_device rd;
            if (delta_energy < 0.0) return true;
            else
            {
                std::uniform_real_distribution<double> dist(0,1);

                if (dist(rd) < exp(-delta_energy / m_boltzman/temperature))
                    return true;
                else return false;
            }
      }

  auto get_spins()
  {
    return spins.submat(1,1,SizeMat(spins.n_rows-2,spins.n_cols-2));
  }


  auto process()
  {
    monte_carlo_step();
  }

  auto process(uint8_t steps)
  {
    for(auto i = 0; i < steps; ++i)
      monte_carlo_step();
  }
};

} // namespace izing_model

#endif // IZING_MODEL_H
