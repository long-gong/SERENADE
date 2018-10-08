//
// Created by saber on 10/8/18.
//

#include <serenade.hpp>
#include <fmt/format.h>

std::vector<int> create_perm(int n){
  if ( n <= 0 ) return {};
  std::vector<int> perm(n, -1);
  for ( int i = 0;i < n;++ i ) perm[i] = (i + 1) % n;
  return perm;
}

std::pair<std::vector<int>/* sr */, std::vector<int> /* sg */> two_matchings(int n, int N){
  auto sg = create_perm(n);
  std::vector<int> sr(N, -1);
  sg.resize(N,-1);
  for ( int i = 0;i < N;++ i ){
    sr[i] = i;
    if ( i >= n ) sg[i] = i;
  }
  return {sr, sg};
}

template <class T>
std::vector<T> vector_from_file(std::string fname){
  std::ifstream in(fname, std::ios::in);
  if (!in.is_open()){
    std::cerr << fmt::format("Cannot open {}", fname) << std::endl;
    return {};
  }
  std::vector<T> vec;
  vec.push_back(-1);
  while ( !in.eof() ) {
    T x(-1);
    in >> x;
    if ( x != -1 ) vec.push_back(x);
  }
  in.close();
  return vec;
}

int main(){
  saber::SERENADE sde;
  std::vector<std::vector<int> > Q(1024, std::vector<int>(1024, 10));

  for ( int N = 8;N <= 1024;N <<= 1 ){
    std::cout << fmt::format("now testing {0}", N) << std::endl;
    auto dfile = fmt::format("../data/n={}.txt", N);
    auto ouroboros = vector_from_file<int>(dfile);
    assert(ouroboros.size() == N + 1);
    auto ofile = fmt::format("bsit_n={}.csv", N);
    std::ofstream out(ofile, std::ios::out);
    if ( !out.is_open() ) {
      std::cerr << fmt::format("Failed to open/create {}", ofile);
      exit(-1);
    }
    out << "cycle-size,binary-search-mpc\n";
    for ( int n = 1; n <= N;++ n ) {
      auto bsit = 0;
      if ( ouroboros[n] == 1 ) {
        auto sr_sg = two_matchings(n, N);
        auto res = sde.emulate(sr_sg.first, sr_sg.second, Q);
        int cs = 0;
        auto &it_vec = std::get<3>(res);
        for ( int i = 0;i < N;++ i ) {
          if ( it_vec[i] > 0 ) {
            ++ cs;
            if ( bsit == 0 ) bsit = it_vec[i];
            else assert( bsit == it_vec[i] );
          }
        }
        assert(cs == n);
      }
      out << fmt::format("{0},{1}\n", n, bsit);
    }
    out.close();
  }
}