//
// Created by saber on 10/8/18.
//

#include <benchmark/benchmark.h>
#include <iostream>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>
#include <fmt/format.h>
#include <random>

using namespace boost::accumulators;


std::tuple<double,double,double,double> acc_measure(int n){
  accumulator_set<double, features< tag::max, tag::mean, tag::min, tag::count > > acc;
  std::mt19937 gen(10890768); //Standard mersenne_twister_engine seeded with rd()
  std::uniform_int_distribution<> dis(1, 9999);
  for (int i = 0;i < n;++ i){
    acc(dis(gen));
  }

  return {max(acc), mean(acc), min(acc), count(acc)};

}

static void BM_AccLargeAmount(benchmark::State& state) {
  for (auto _ : state)
    acc_measure(int(1e10));
}
// Register the function as a benchmark
BENCHMARK(BM_AccLargeAmount);

BENCHMARK_MAIN();
//int main(){
//  int n = int(1e6);
//  auto res = acc_measure(n);
//  std::cout << fmt::format("max: {0:.0f}\nmean: {1:.4f}\nmin: {2:.0f}\ncount: {3:.0f}\n\n", std::get<0>(res),
//      std::get<1>(res), std::get<2>(res), std::get<3>(res));
//
//  return 0;
//}
