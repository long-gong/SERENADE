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
#include <csv_utils.hpp>
#include <fmt/format.h>

using namespace arma;

TEST(ArmadilloTest, Constructor) {
  int n = 5;
  mat A = randu<mat>(n, n);
  A.print("A = ");
  EXPECT_TRUE(true);
}

TEST(VectorFillTest, Resize) {
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

  for (int i = 0; i < n; ++i)
    perm[i] = i;
  std::copy(perm.begin(), perm.end(), sr.begin());
  std::random_shuffle(perm.begin(), perm.end());
  std::copy(perm.begin(), perm.end(), sg.begin());

  std::cout << "Queue lengths:\n";
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      Q[i][j] = int(A.at(i, j) * max_l);
      if (sr[i] == j || sg[i] == j)
        std::cout << i << " " << j << " : " << Q[i][j] << std::endl;
    }
  }
  std::cout << std::endl;

  saber::SERENADE sde;
  auto res = sde.run(sr, sg, Q);
  std::cout << "decisions: ";
  for (const auto &d : std::get<0>(res))
    std::cout << d << " ";
  std::cout << std::endl;
  std::cout << "cycles: ";
  for (const auto &c : std::get<1>(res))
    std::cout << c << " ";
  std::cout << std::endl;
  std::cout << "cycle lengths: ";
  for (const auto &l : std::get<2>(res))
    std::cout << l << " ";
  std::cout << std::endl;

  saber::SERENAGraphBased sgb;
  auto res2 = sgb(sr, sg, Q);

  std::cout << "\n\ndecisions: ";
  for (const auto &d : std::get<0>(res2))
    std::cout << d << " ";
  std::cout << std::endl;
  std::cout << "cycles: ";
  for (const auto &c : std::get<1>(res2))
    std::cout << c << " ";
  std::cout << std::endl;
  std::cout << "cycle lengths: ";
  for (const auto &l : std::get<2>(res2))
    std::cout << l << " ";
  std::cout << std::endl;
}

TEST(SerenaTest, LargeScale) {
  int n = 100;
  std::vector<int> perm(n, -1);
  std::vector<int> sr(n, -1);
  std::vector<int> sg(n, -1);
  std::vector<std::vector<int> > Q(n, std::vector<int>(n, 0));
  const int max_l = 10;

  saber::SERENADE sde;
  saber::SERENAGraphBased sgb;

  mat A = randu<mat>(n, n);
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      Q[i][j] = int(A.at(i, j) * max_l);
    }
  }

  std::random_device rd;
  std::mt19937 g(rd());
  int T = 10000;
  for (int t = 0; t < T; ++t) {
    for (int i = 0; i < n; ++i)
      perm[i] = i;
    std::copy(perm.begin(), perm.end(), sr.begin());
    std::shuffle(perm.begin(), perm.end(), g);
    std::copy(perm.begin(), perm.end(), sg.begin());

    auto res1 = sde.run(sr, sg, Q);
    auto res2 = sgb(sr, sg, Q);
    EXPECT_EQ(std::get<0>(res1), std::get<0>(res2));
    auto &cycles1 = std::get<1>(res1);
    auto &cycles2 = std::get<1>(res2);
    auto &cycle_l1 = std::get<2>(res1);
    auto &cycle_l2 = std::get<2>(res2);

    for (int i = 0; i < n; ++i)
      EXPECT_EQ(cycle_l1[cycles1[i]], cycle_l2[cycles2[i]]);
  }
}

TEST(SerenadeTest, KnowledgeDiscSmall) {
  int n = 8;
  std::vector<int> perm(n, -1);
  std::vector<int> sr(n, -1);
  std::vector<int> sg(n, -1);
  std::vector<std::vector<int> > Q(n, std::vector<int>(n, 0));
  const int max_l = 10;

  mat A = randu<mat>(n, n);
  std::random_device rd;
  std::mt19937 g(rd());
  for (int i = 0; i < n; ++i)
    perm[i] = i;
  std::copy(perm.begin(), perm.end(), sr.begin());
  std::shuffle(perm.begin(), perm.end(), g);
  std::copy(perm.begin(), perm.end(), sg.begin());

  std::cout << "Queue lengths:\n";
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      Q[i][j] = int(A.at(i, j) * max_l);
      if (sr[i] == j || sg[i] == j)
        std::cout << i << " " << j << " : " << Q[i][j] << std::endl;
    }
  }
  std::cout << std::endl;

  saber::SERENADE sde;
  auto res = sde.run(sr, sg, Q);
  std::cout << "decisions: ";
  for (const auto &d : std::get<0>(res))
    std::cout << d << " ";
  std::cout << std::endl;
  std::cout << "cycles: ";
  for (const auto &c : std::get<1>(res))
    std::cout << c << " ";
  std::cout << std::endl;
  std::cout << "cycle lengths: ";
  for (const auto &l : std::get<2>(res))
    std::cout << l << " ";
  std::cout << std::endl;

  auto res2 = sde.emulate(sr, sg, Q, true);
  std::cout << "decisions: ";
  for (const auto &d : std::get<0>(res2))
    std::cout << d << " ";
  std::cout << std::endl;
  std::cout << "iterations: ";
  for (const auto &it : std::get<1>(res2))
    std::cout << it << " ";
  std::cout << std::endl;
}


// Note that to do this test, you MUST define TEST_EXTREME_LARGE_CASES
TEST(SerenadeTest, KnowledgeDiscLarge) {
  int n = 1024;
  std::vector<int> perm(n, -1);
  std::vector<int> sr(n, -1);
  std::vector<int> sg(n, -1);
  std::vector<std::vector<int> > Q(n, std::vector<int>(n, 0));
  const int max_l = 10;

  mat A = randu<mat>(n, n);
  auto seed = size_t(1e9);
  std::mt19937_64 g(seed);

  std::string csvfile("../data/iterations_def.csv");
  saber::CsvReader cr(csvfile);
  cr.next();// skip header
  auto vec = cr.next();
  std::vector<int> its(1025, 0);
  while (!vec.empty()) {
    its[std::stoi(vec[0])] = std::stoi(vec[1]);
    vec = cr.next();
  }

  saber::SERENADE sde;

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      Q[i][j] = int(A.at(i, j) * max_l);
    }
  }

  int T = 10000;

  for (int t = 0; t < T; ++t) {

//    if ( t >= 108 ) {
//      std::cout << t << std::endl;
//    }
    for (int i = 0; i < n; ++i) perm[i] = i;
    std::copy(perm.begin(), perm.end(), sr.begin());
    std::shuffle(perm.begin(), perm.end(), g);
    std::copy(perm.begin(), perm.end(), sg.begin());


//    if ( t > 108 ) {
//      std::cout << "before bench" << std::endl;
//    }
    auto res = sde.run(sr, sg, Q);
    // std::cout << "bench finished\n" << std::endl;
    auto res2 = sde.emulate(sr, sg, Q, true);
    // std::cout << "me finished\n" << std::endl;


    EXPECT_EQ(std::get<0>(res), std::get<0>(res2));
    auto &cycle_lens = std::get<2>(res);
    auto &cycles = std::get<1>(res);
    auto &iterations = std::get<1>(res2);
    auto &cycle_weights = std::get<3>(res);

    for (int i = 0; i < n; ++i) {
      ASSERT_TRUE(cycles[i] < cycle_lens.size());
      ASSERT_TRUE(cycle_lens[cycles[i]] < 1025);
      auto it2 = its[cycle_lens[cycles[i]]];
      EXPECT_EQ(it2, iterations[i]);
    }
    if ((t + 1) % 100 == 0) {
      std::cout << (t + 1) << "/" << T << " finished\n";
    }
  }

}

TEST(SerenadeTest, ExactEmu){
  int n = 1024;
  std::vector<int> perm(n, -1);
  std::vector<int> sr(n, -1);
  std::vector<int> sg(n, -1);
  std::vector<std::vector<int> > Q(n, std::vector<int>(n, 0));
  const int max_l = 20;

  mat A = randu<mat>(n, n);
  auto seed = size_t(1e9);
  std::mt19937_64 g(seed);

  std::string csvfile("../data/iterations_def.csv");
  saber::CsvReader cr(csvfile);
  cr.next();// skip header
  auto vec = cr.next();
  std::vector<int> its(1025, 0);
  while (!vec.empty()) {
    its[std::stoi(vec[0])] = std::stoi(vec[1]);
    vec = cr.next();
  }

  std::string ofile = fmt::format("../data/n={0}.txt", n);

  std::ifstream in(ofile, std::ios::in);
  if ( !in.is_open() ) std::cerr << fmt::format("Can not open {0}", ofile) << std::endl;
  std::vector<int> ourob;
  ourob.push_back(-1);
  while (!in.eof()) {
    int x;
    in >> x;
    ourob.push_back(x);
  }
//  for ( const auto x: ourob) std::cout << x << " ";
//  std::cout << std::endl;

  saber::SERENADE sde;

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      Q[i][j] = int(A.at(i, j) * max_l);
    }
  }

  int T = 10000;

  for ( int t = 0; t < T;++ t ) {
    for (int i = 0; i < n; ++i) perm[i] = i;
    std::copy(perm.begin(), perm.end(), sr.begin());
    std::shuffle(perm.begin(), perm.end(), g);
    std::copy(perm.begin(), perm.end(), sg.begin());

    auto res1 = sde.run(sr, sg, Q);
    auto res2 = sde.emulate(sr, sg, Q);

    EXPECT_EQ(std::get<0>(res1), std::get<0>(res2));
    auto &cycle_lens = std::get<2>(res1);
    auto &cycles = std::get<1>(res1);
    //auto &iterations = std::get<1>(res2);
    //auto &cycle_weights = std::get<3>(res1);
    auto &cycle_types = std::get<2>(res2);
    for ( int i = 0;i < n;++ i) {
      int c = cycles[i];
      ASSERT_TRUE(c < n);
      EXPECT_EQ(cycle_types[i], ourob[cycle_lens[c]]);
      if ( cycle_types[i] != ourob[cycle_lens[c]]){
        std::cout << cycle_lens[c] << std::endl;
      }
    }
  }
}


TEST(SerenadeTest, Approx){
  int n = 1024;
  std::vector<int> perm(n, -1);
  std::vector<int> sr(n, -1);
  std::vector<int> sg(n, -1);
  std::vector<std::vector<int> > Q(n, std::vector<int>(n, 0));
  const int max_l = 20;

  mat A = randu<mat>(n, n);
  auto seed = size_t(1e9);
  std::mt19937_64 g(seed);

  std::string csvfile("../data/iterations_def.csv");
  saber::CsvReader cr(csvfile);
  cr.next();// skip header
  auto vec = cr.next();
  std::vector<int> its(1025, 0);
  while (!vec.empty()) {
    its[std::stoi(vec[0])] = std::stoi(vec[1]);
    vec = cr.next();
  }

  std::string ofile = fmt::format("../data/n={0}.txt", n);

  std::ifstream in(ofile, std::ios::in);
  if ( !in.is_open() ) std::cerr << fmt::format("Can not open {0}", ofile) << std::endl;
  std::vector<int> ourob;
  ourob.push_back(-1);
  while (!in.eof()) {
    int x;
    in >> x;
    ourob.push_back(x);
  }


  saber::SERENADE sde;

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      Q[i][j] = int(A.at(i, j) * max_l);
    }
  }

  int T = 1;

  for ( int t = 0; t < T;++ t ) {
    for (int i = 0; i < n; ++i) perm[i] = i;
    std::copy(perm.begin(), perm.end(), sr.begin());
    std::shuffle(perm.begin(), perm.end(), g);
    std::copy(perm.begin(), perm.end(), sg.begin());

    auto res1 = sde.run_approx(sr, sg, Q, ourob);
    auto res2 = sde.approx(sr, sg, Q);

    EXPECT_EQ(std::get<0>(res1), std::get<0>(res2));

  }
}


TEST(SerenadeTest, Bsit){
  int n = 128;
  std::vector<int> perm(n, -1);
  std::vector<int> sr(n, -1);
  std::vector<int> sg(n, -1);
  std::vector<std::vector<int> > Q(n, std::vector<int>(n, 0));
  const int max_l = 20;

  mat A = randu<mat>(n, n);
  auto seed = size_t(1e9);
  std::mt19937_64 g(seed);

  std::string csvfile("../data/iterations_def.csv");
  saber::CsvReader cr(csvfile);
  cr.next();// skip header
  auto vec = cr.next();
  std::vector<int> its(1025, 0);
  while (!vec.empty()) {
    its[std::stoi(vec[0])] = std::stoi(vec[1]);
    vec = cr.next();
  }

  std::string ofile = fmt::format("../data/n={0}.txt", n);

  std::ifstream in(ofile, std::ios::in);
  if ( !in.is_open() ) std::cerr << fmt::format("Can not open {0}", ofile) << std::endl;
  std::vector<int> ourob;
  ourob.push_back(-1);
  while (!in.eof()) {
    int x;
    in >> x;
    ourob.push_back(x);
  }
//  for ( const auto x: ourob) std::cout << x << " ";
//  std::cout << std::endl;

  saber::SERENADE sde;

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      Q[i][j] = int(A.at(i, j) * max_l);
    }
  }

  int T = 1;

  for ( int t = 0; t < T;++ t ) {
    for (int i = 0; i < n; ++i) perm[i] = i;
    std::copy(perm.begin(), perm.end(), sr.begin());
    std::shuffle(perm.begin(), perm.end(), g);
    std::copy(perm.begin(), perm.end(), sg.begin());

    auto res1 = sde.run(sr, sg, Q);
    auto res2 = sde.emulate(sr, sg, Q);

    EXPECT_EQ(std::get<0>(res1), std::get<0>(res2));
    auto &cycle_lens = std::get<2>(res1);
    auto &cycles = std::get<1>(res1);
    auto &bsit = std::get<3>(res2);
    //auto &iterations = std::get<1>(res2);
    //auto &cycle_weights = std::get<3>(res1);
    auto &cycle_types = std::get<2>(res2);
    for ( int i = 0;i < n;++ i) {
      int c = cycles[i];
      ASSERT_TRUE(c < n);
      std::cout << cycle_lens[c] << " : " << bsit[i] << std::endl;
    }
  }
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}