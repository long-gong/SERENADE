//
// Created by saber on 10/6/18.
//

#include <iostream>
#include <armadillo>
#include <gtest/gtest.h>
#include <random>
#include <algorithm>
#include <iterator>
#include <serenade.hpp>
#include <serena_graph_based.hpp>

using namespace arma;


TEST(ArmadilloTest, Constructor){
  int n = 5;
  mat A = randu<mat>(n, n);
  A.print("A = ");
  EXPECT_TRUE(true);
}

TEST(VectorFillTest, Resize)
{
  std::vector<int> a(2, 2);
  a.resize(4, -1);
}

TEST(SerenaTest, MergeCheck) {
    int n = 8;
    std::vector<int> perm(n, -1);
    std::vector<int> sr(n, -1);
    std::vector<int> sg(n, -1);
    std::vector<std::vector<int> > Q(n, std::vector<int>(n, 0));
    const int max_l = 10;

    mat A = randu<mat>(n, n);

    for ( int i = 0;i < n;++i ) perm[i] = i;
    std::copy(perm.begin(), perm.end(), sr.begin());
    std::random_shuffle(perm.begin(), perm.end());
    std::copy(perm.begin(), perm.end(), sg.begin());

    std::cout << "Queue lengths:\n";
    for ( int i = 0;i < n;++ i) {
      for ( int j = 0;j < n;++ j) {
        Q[i][j] = int(A.at(i,j) * max_l);
        if ( sr[i] == j || sg[i] == j ) std::cout << i << " " << j << " : " << Q[i][j] << std::endl;
      }
    }
    std::cout << std::endl;

    saber::SERENADE sde;
    auto res = sde.run(sr, sg, Q);
    std::cout << "decisions: " ;
    for ( const auto& d : std::get<0>(res) ) std::cout << d << " ";
    std::cout << std::endl;
    std::cout << "cycles: " ;
    for ( const auto& c : std::get<1>(res) ) std::cout << c << " ";
    std::cout << std::endl;
    std::cout << "cycle lengths: " ;
    for ( const auto& l : std::get<2>(res) ) std::cout << l << " ";
    std::cout << std::endl;

    saber::SERENAGraphBased sgb;
    auto res2 = sgb(sr, sg, Q);

    std::cout << "\n\ndecisions: " ;
    for ( const auto& d : std::get<0>(res2) ) std::cout << d << " ";
    std::cout << std::endl;
    std::cout << "cycles: " ;
    for ( const auto& c : std::get<1>(res2) ) std::cout << c << " ";
    std::cout << std::endl;
    std::cout << "cycle lengths: " ;
    for ( const auto& l : std::get<2>(res2) ) std::cout << l << " ";
    std::cout << std::endl;
}


TEST(SerenaTest, LargeScale){
  int n = 100;
  std::vector<int> perm(n, -1);
  std::vector<int> sr(n, -1);
  std::vector<int> sg(n, -1);
  std::vector<std::vector<int> > Q(n, std::vector<int>(n, 0));
  const int max_l = 10;

  saber::SERENADE sde;
  saber::SERENAGraphBased sgb;

  mat A = randu<mat>(n, n);
  for ( int i = 0;i < n;++ i) {
    for ( int j = 0;j < n;++ j) {
      Q[i][j] = int(A.at(i,j) * max_l);
    }
  }

  int T = 10000;
  for ( int t = 0;t < T;++ t) {
    for (int i = 0; i < n; ++i)
      perm[i] = i;
    std::copy(perm.begin(), perm.end(), sr.begin());
    std::random_shuffle(perm.begin(), perm.end());
    std::copy(perm.begin(), perm.end(), sg.begin());

    auto res1 = sde.run(sr, sg, Q);
    auto res2 = sgb(sr, sg, Q);
    EXPECT_EQ(std::get<0>(res1), std::get<0>(res2));
    auto& cycles1 = std::get<1>(res1);
    auto& cycles2 = std::get<1>(res2);
    auto& cycle_l1 = std::get<2>(res1);
    auto& cycle_l2 = std::get<2>(res2);

    for ( int i = 0;i < n;++ i)
      EXPECT_EQ(cycle_l1[cycles1[i]], cycle_l2[cycles2[i]]);
  }
}

int main(int argc, char* argv[]){
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}