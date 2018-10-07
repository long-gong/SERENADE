

#ifndef SERENADE_SERENADE_GRAPH_BASED_HPP
#define SERENADE_SERENADE_GRAPH_BASED_HPP

#include <boost/config.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>
#include <tuple>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>


namespace saber {
class SERENAGraphBased {
public:
  typedef std::vector<int> perm_t;
  typedef std::vector <std::vector<int>> matrix_t;
  std::tuple<std::vector<int> /* decisions */, std::vector<int> /* which cycle each vertex is on */, std::vector<int> /* cycle lengths */ >
      operator()(perm_t &sr, perm_t &sg, matrix_t &Q);
};
std::tuple<std::vector<int> /* decisions */,
           std::vector<int> /* which cycle each vertex is on */,
           std::vector<int> /* cycle lengths */ > SERENAGraphBased::operator()(SERENAGraphBased::perm_t &sr,
                                                                               SERENAGraphBased::perm_t &sg,
                                                                               SERENAGraphBased::matrix_t &Q) {
  using namespace boost;
  {
    typedef adjacency_list <vecS, vecS, undirectedS> Graph;
    Graph g;
    auto n = sr.size();
    for ( int i = 0;i < n;++ i)
      add_edge(i, sr[i] + n, g);
    for ( int i = 0;i < n;++ i)
      add_edge(i, sg[i] + n, g);
    std::vector<int> component(num_vertices(g));
    int num = connected_components(g, &component[0]);

    std::vector<int> decisions(n, -1);
    std::vector<int> cycle_lens(num, 0);
    std::vector<int> wr(num, 0);
    std::vector<int> wg(num, 0);
    for ( int i = 0;i < n;++ i) {
      auto c = component[i];
      ++ cycle_lens[c];
      wr[c] += Q[i][sr[i]];
      wg[c] += Q[i][sg[i]];
    }
    for ( int i = 0;i < n;++ i){
      auto c = component[i];
      decisions[i] = (wr[c] >= wg[c] ? 1 : 0);
    }
    component.resize(n);
    return {decisions, component, cycle_lens};

  }
}
}
#endif //SERENADE_SERENADE_GRAPH_BASED_HPP
