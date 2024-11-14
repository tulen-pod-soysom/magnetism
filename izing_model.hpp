#ifndef IZING_MODEL_H
#define IZING_MODEL_H

#include <armadillo>
#include <cstdint>
#include <random>

namespace izing_model {

using namespace arma;

static std::random_device rd;

struct model {
  Mat<int8_t> spins;

  double J = 1;
  double temperature;
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
    spins = Mat<int8_t>(w+2,h+2);
    
    std::uniform_int_distribution<int> dist(0,1);

    for (auto i = 1; i < w+1; ++i)
      for (auto j = 1; j < h+1; ++j)
      {
          auto a = (dist(rd) == 1)? -1:1;
          spins(i,j) = a;
      }

    use_periodic_boundary();
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

  }

  template <typename Function>
  auto for_each_spin(Function f)
  {
    for (auto i = 1; i < w+1; ++i)
      for (auto j = 1; j < h+1; ++j)
        f(spins(i,j));
  }

  auto get_summary_spin()
  {
    int spin = 0;
    for_each_spin([&](auto& a){spin += a;});
    return spin;
  }

  auto get_full_energy()
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

      s0 = -s0;
      switch (v[dist(rd)]) {
        case 1: s1 = -s1; break;
        case 2: s2 = -s2; break;
        case 3: s3 = -s3; break;
        case 4: s4 = -s4; break;
      }

    }
    else
    {
      throw; // todo later
    }
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
