#ifndef IZING_MODEL_H
#define IZING_MODEL_H

#include <armadillo>
#include <cstdint>
#include <random>

namespace izing_model 
{

using namespace arma;

static std::random_device rd;

struct model 
{
public: // variables
  double J = 1;
  constexpr static double m_boltzman = 1.38E-23;
  double temperature =0 ;
  double energy = 0;
  int w,h;

private: // variables

  Mat<char> spins;
  std::uniform_int_distribution<int>* dist_w = nullptr;
  std::uniform_int_distribution<int>* dist_h = nullptr;
  std::uniform_int_distribution<int>* dist_bin = nullptr;
  std::uniform_int_distribution<int>* dist_direction = nullptr;
  std::uniform_real_distribution<double>* double_dist = nullptr;

private: // functions

  auto delta_energy(int i1, int j1, int i2, int j2)
  {
      double energy = 0;
      auto s1 = -spins(i1,j1);
      auto s2 = -spins(i2,j2);

      energy += s1 * (spins(i1 - 1,j1) + spins(i1 + 1,j1) + spins(i1,j1 - 1) + spins(i1,j1 + 1));
      energy += s2 * (spins(i2 - 1,j2) + spins(i2 + 1,j2) + spins(i2,j2 - 1) + spins(i2,j2 + 1));
      
      if (!((abs(i1 - i2) > 1) || (abs(j1 - j2) > 1)))
          energy += 3*s1*s2; // two to correct upper two lines, third is real energy

      return - J * energy;
  }

//   using kawasaki dynamic
  auto chenge_system_state(bool only_neighbours = true)
  {
    auto i1 = (*dist_w)(rd);
    auto j1 = (*dist_h)(rd);

    if (only_neighbours)
    {
      int dir = (*dist_direction)(rd);
      int i2 = i1;
      int j2 = j1;

      if (dir == 0)      // left
        i2 -= 1;
      else if (dir == 1) // right
        i2 += 1;
      else if (dir == 2) // top
        j2 += 1;
      else               // bot
        j2 -= 1;

      if (i2 == 0)           // left bourder
        i2 = w;
      else if (i2 == w + 1)  // right bourder
        i2 = 1;
      
      if (j2 == 0)           // bot bourder
        j2 = h;
      else if (j2 == h + 1)  // top bourder
        j2 = 1;

      if (spins(i1, j1) == spins(i2, j2)) // s1 == s2
        return;

      double de = delta_energy(i1, j1, i2, j2);

      if (metropolis_algorithm(de))
      {
         std::swap(spins(i1,j1), spins(i2, j2));
      }
    }
    else
    {
      // throw; // todo later
    }

    use_periodic_boundary();

    return;
  }

  bool metropolis_algorithm(double delta_energy)
  {
    if (delta_energy < 0.0)
      return true;

    if ((*double_dist)(rd) < exp(-delta_energy / J / temperature)) 
      return true;
    
    return false;
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

public: // functions

// Destructor
~model()
{
    if (dist_w != nullptr)
        delete dist_w;
    if (dist_h != nullptr)
        delete dist_h;
    if (dist_bin != nullptr)
        delete dist_bin;
    if (dist_direction != nullptr)
        delete dist_direction;
    if (double_dist != nullptr)
        delete double_dist;
}

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
    dist_w = new std::uniform_int_distribution<int>(1, w);
    dist_h = new std::uniform_int_distribution<int>(1, h);
    dist_bin = new std::uniform_int_distribution<int>(0, 1);
    dist_direction = new std::uniform_int_distribution<int>(0, 3);
    double_dist = new std::uniform_real_distribution<double>(0, 1);
  
    this->w = w;
    this->h = h;
    spins = Mat<char>(w+2,h+2);
    
    for (auto i = 1; i < w + 1; ++i)
      for (auto j = 1; j < h + 1; ++j)
      {
          if (i <= w / 2)
              spins(i,j) = 1;
          else
              spins(i,j) = -1;
      }

    int i1, i2, j1, j2;
    for (int i = 0; i < (w + 1) * (h + 1); ++i)
    {
      i1 = (*dist_w)(rd);
      i2 = (*dist_w)(rd);
      j1 = (*dist_h)(rd);
      j2 = (*dist_h)(rd);

      std::swap(spins(i1, j1), spins(i2, j2));
    }

    use_periodic_boundary();
    energy = get_full_energy();
  }

  void get_spins_statistic(int& num_plus, int& num_minus)
  {
      num_plus = 0;
      num_minus = 0;

      for (int  i = 1; i < w + 1; ++i)
      {
          for (int j = 1; j < h + 1; ++j)
          {
              if (spins(i,j) == 1)
                  num_plus++;
              else
                  num_minus++;
          }
      }
      return;
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

  auto get_spins()
  {
    return spins.submat(1,1,SizeMat(spins.n_rows-2,spins.n_cols-2));
  }

  auto process()
  {
    chenge_system_state();
  }

  auto process(uint8_t steps)
  {
    for(auto i = 0; i < steps; ++i)
      chenge_system_state();
  }

};

} // namespace izing_model

#endif // IZING_MODEL_H
