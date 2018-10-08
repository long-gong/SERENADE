
#ifndef PROJECT_SERENADE_HPP
#define PROJECT_SERENADE_HPP
#include <vector>
#include <fstream>
#include <utility>
#include <tuple>
#include <cmath>
#include <cassert>
#include <iostream>


#define TEST_EXTREME_LARGE_CASES

#ifdef TEST_EXTREME_LARGE_CASES
#include <boost/multiprecision/gmp.hpp>
using namespace boost::multiprecision;
#endif

namespace saber {
struct KnowledgeSet {
  int id;
#ifdef TEST_EXTREME_LARGE_CASES
  mpz_int wr;
  mpz_int wg;
#else
  long long wr;
  long long wg;
#endif
  int leader;
  KnowledgeSet(int id, int wr, int wg, int leader) : id(id), wr(wr), wg(wg), leader(leader) {}
  KnowledgeSet(int id, int wr, int wg) : id(id), wr(wr), wg(wg), leader(-1) {}
#ifdef TEST_EXTREME_LARGE_CASES
  KnowledgeSet(int id, mpz_int wr, mpz_int wg, int leader) : id(id), wr(wr), wg(wg), leader(leader) {}
  KnowledgeSet(int id, mpz_int wr, mpz_int wg) : id(id), wr(wr), wg(wg), leader(-1) {}
#endif
};

class SERENADE {
public:
  typedef std::vector<int> perm_t;
  typedef std::vector<std::vector<int>> matrix_t;
protected:
  perm_t dummy_perm;
  perm_t _sg;
  perm_t _sr;
  int _port_num;
  int _max_iter;
  perm_t _sigma;
  perm_t _sigma_inv;
  perm_t _decisions;
  std::vector<std::vector<KnowledgeSet>> _knowledge_sets;
  std::vector<std::vector<int>> _B;
  std::vector<std::vector<KnowledgeSet>> _receive;
public:
  SERENADE() : _sg(dummy_perm), _sr(dummy_perm), _port_num(-1), _max_iter(-1) {}
  std::tuple<std::vector<int> /* decisions */,
             std::vector<int> /* # of iterations */,
             std::vector<int> /* cycle types */,
             int /* # of non-ouroboros */> emulate(const perm_t &sr, const perm_t &sg, const matrix_t &Q, bool no_bs = false);
  std::tuple<std::vector<int> /* decisions */,
             std::vector<int> /* # of iterations */,
             std::vector<int> /* cycle types */,
             int /* # of non-ouroboros */> approx(const perm_t &sr, const perm_t &sg, const matrix_t &Q);
  std::tuple<std::vector<int> /* decisions */,
             std::vector<int> /* which cycle each vertex is on */,
             std::vector<int> /* cycle lengths */,
             std::vector<std::pair<int, int>> /* weights */> run(const perm_t &sr, const perm_t &sg, const matrix_t &Q);
  std::tuple<std::vector<int> /* decisions */,
             std::vector<int> /* which cycle each vertex is on */,
             std::vector<int> /* cycle lengths */,
             std::vector<std::pair<int, int>> /* weights */> run_approx(const perm_t &sr, const perm_t &sg, const matrix_t &Q, const std::vector<int>& ourobors);
private:
  void init(const perm_t &sr, const perm_t &sg);
  void knowledge_disc(const matrix_t &Q, int max_iter, std::vector<int> &iterations);
  void knowledge_disc_zero(int i, const matrix_t &Q);
  void knowledge_disc_send(int i, int k);
  void knowledge_disc_receive(int i, int k);
  void knowledge_disc_halt_check(int i, int k);

#ifdef TEST_EXTREME_LARGE_CASES
  int binary_search(int i, int k, mpz_int wr, mpz_int wg, int leader);
#else
  int binary_search(int i, int k, int wr, int wg, int leader);
#endif

  KnowledgeSet phi(int i, int k);
  KnowledgeSet phi_opposite(int i, int k);

};

/*
 * Initialization
 */
void SERENADE::init(const std::vector<int> &sr, const std::vector<int> &sg) {
  _sg.resize(sg.size());
  _sr.resize(sr.size());
  std::copy(sg.begin(), sg.end(), _sg.begin());
  std::copy(sr.begin(), sr.end(), _sr.begin());

  if (_port_num != sr.size()) {
    _port_num = sr.size();
    _max_iter = int(std::ceil(std::log2(_port_num)));
    _sigma.resize(_port_num, -1);
    _sigma_inv.resize(_port_num, -1);
    _decisions.resize(_port_num, -1);

    _knowledge_sets.resize(_port_num);
    _B.resize(_port_num, std::vector<int>(_port_num, -1));
    _receive.resize(_port_num);
  }

  // Actually, there are optimization spaces here, since B only has O(log N) non-null elements.
  std::fill(_decisions.begin(), _decisions.end(), -1);
  for (int i = 0; i < _port_num; ++i) {
    _knowledge_sets[i].clear();
    std::fill(_B[i].begin(), _B[i].end(), -1);
    _receive[i].clear();
  }

  std::vector<int> sg_inv(_port_num, -1);
  for ( int i = 0;i < _port_num;++ i ) {
    auto o = sg[i];
    assert(sg_inv[o] == -1);
    sg_inv[o] = i;
  }
  for (int i = 0; i < _port_num; ++i) {
    auto i_prime = sg_inv[sr[i]];
    _sigma[i] = i_prime;
    _sigma_inv[i_prime] = i;
  }
}

std::tuple<std::vector<int> /* decisions */,
           std::vector<int> /* # of iterations */,
           std::vector<int> /* cycle types */,
           int /* # of non-ouroboros */>
SERENADE::emulate(const perm_t &sr, const perm_t &sg, const matrix_t &Q, bool no_bs) {

  init(sr, sg);

  std::vector<int> iterations(_port_num, 1);
  std::vector<int> type_of_cycles(_port_num, 0);

  if (no_bs) {
    knowledge_disc(Q, _port_num / 2, iterations);
    return {_decisions, iterations, std::vector<int>(), 0};
  } else
    knowledge_disc(Q, _max_iter, iterations);

  // binary search
  int n_cnt = 0;
  int k = _max_iter;
  for (int i = 0; i < _port_num; ++i) {
    if (_decisions[i] == -1) {
      auto ks = phi_opposite(i, k);
      auto l = ks.leader;
      if (l == ks.id) {// the initial search admin
        _decisions[l] = binary_search(i, k, ks.wr, ks.wg, l);
        ++n_cnt;
      }
      type_of_cycles[i] = 1;
    }
  }

  for (int i = 0; i < _port_num; ++i) {
    if (_decisions[i] == -1) {
      auto ks = phi_opposite(i, k);
      auto l = ks.leader;
      _decisions[i] = _decisions[l];
    }
  }

  return {_decisions, iterations, type_of_cycles, n_cnt};

}

/*
 * The 0th iteration of SERENADE
 */
void SERENADE::knowledge_disc_zero(int i, const matrix_t &Q) {
  auto i_d = _sigma[i];
  auto i_u = _sigma_inv[i];

  _knowledge_sets[i].emplace_back(i_d, Q[i][_sr[i]], Q[i_d][_sg[i_d]], -1);
  _knowledge_sets[i].emplace_back(i_u, Q[i_u][_sr[i_u]], Q[i][_sg[i]], std::min(i_u, i));
  // check whether i has discovered some vertex twice
  if (i_d == i) {
    // cycle with size of 1
    _decisions[i] = (_knowledge_sets[i].back().wr >= _knowledge_sets[i].back().wg ? 1 : 0);
    return;
  } else if (i_d == i_u) {
    // cycle with size of 2
    auto phi_k = phi(i, 0);
    auto phi_op_k = phi_opposite(i, 0);
    _decisions[i] = ( (phi_k.wr + phi_op_k.wr) >= (phi_k.wg + phi_op_k.wg) ? 1 : 0);
    return;
  }

  _B[i][i_d] = 0;
  _B[i][i_u] = 1;
}

/*
 * The k^{th} iteration: sending
 */
void SERENADE::knowledge_disc_send(int i, int k) {
  auto ks = phi(i, k - 1);
  auto ks_op = phi_opposite(i, k - 1);
  auto i_d = ks.id;
  auto i_u = ks_op.id;
  _receive[i_d].push_back(ks_op);
  _receive[i_u].push_back(ks);
}

/*
 * The k^{th} iteration: receiving
 */
void SERENADE::knowledge_disc_receive(int i, int k) {
  auto ks_local = phi(i, k - 1);
  auto ks_local_op = phi_opposite(i, k - 1);

  if (_receive[i].size() != 2) {
    std::cerr << "error: " << i << " " << k << " " << _sigma[i] << " " << _sigma_inv[i] << " " << _receive[i].size()
              << std::endl;
    std::cout << std::endl;
    for (int j = 0; j < _port_num; ++j) {
      std::cout << j << " : " << _receive[j].size() << std::endl;
    }
    std::cout << std::endl;
  }

  assert(_receive[i].size() == 2);
  assert(!((_receive[i][0].leader == -1 && _receive[i][1].leader == -1) || (_receive[i][0].leader != -1 && _receive[i][1].leader != -1)));
  auto ks_from_d = (_receive[i][0].leader == -1) ? _receive[i][0] : _receive[i][1];
  auto ks_from_u = (_receive[i][0].leader != -1) ? _receive[i][0] : _receive[i][1];

  // calculate the new knowledge sets
  _knowledge_sets[i].emplace_back(ks_from_d.id, ks_from_d.wr + ks_local.wr, ks_local.wg + ks_from_d.wg, -1);
  _knowledge_sets[i].emplace_back(ks_from_u.id, ks_from_u.wr + ks_local_op.wr,
                                  ks_local_op.wg + ks_from_u.wg, std::min(ks_from_u.leader, ks_local_op.leader));
  _receive[i].clear();
}

/*
 * The k^{th} iteration: halt checking
 */
void SERENADE::knowledge_disc_halt_check(int i, int k) {
  auto ks1 = phi(i, k);
  auto ks2 = phi_opposite(i, k);

  auto i_d = ks1.id;
  auto i_u = ks2.id;

  if (_B[i][i_d] != -1) {
    // rediscover
    auto ks = _knowledge_sets[i][_B[i][i_d]];
    if (ks.leader == -1) {
      // same direction
      _decisions[i] = ((ks1.wr - ks.wr) >= (ks1.wg - ks.wg) ? 1 : 0);
    } else {
      // opposite direction
      _decisions[i] = ((ks1.wr + ks.wr) >= (ks1.wg + ks.wg) ? 1 : 0);
    }
    return;
  }
  else if (_B[i][i_u] != -1) {
    // rediscover
    auto ks = _knowledge_sets[i][_B[i][i_u]];
    if (ks.leader != -1) {
      // same direction
      _decisions[i] = ((ks2.wr - ks.wr) >= (ks2.wg - ks.wg) ? 1 : 0);
    } else {
      // opposite direction
      _decisions[i] = ((ks2.wr + ks.wr) >= (ks2.wg + ks.wg) ? 1 : 0);
    }
    return;
  }
  else if (i_d == i_u) {
    _decisions[i] = ((ks1.wr + ks2.wr) >= (ks1.wg + ks2.wg) ? 1 : 0);
    return;
  }

  _B[i][i_d] = 2 * k;
  _B[i][i_u] = 2 * k + 1;
}

#ifdef TEST_EXTREME_LARGE_CASES
int SERENADE::binary_search(int i, int k, mpz_int wr, mpz_int wg, const int leader) {
#else
int SERENADE::binary_search(int i, int k, int wr, int wg, const int leader) {
#endif
  if (i == leader) {
    return (wr >= wg ? 1 : 0);
  }

  int decision = -1;
  auto ks = phi_opposite(i, k - 1);

  auto mid = ks.id;
  if (mid == leader) {
    wr -= ks.wr;
    wg -= ks.wg;
    decision = binary_search(mid, k - 1, wr, wg, leader);
  } else {
    if (ks.leader == leader) {
      decision = binary_search(i, k - 1, wr, wg, leader);
    } else {
      wg -= ks.wg;
      wr -= ks.wr;
      decision = binary_search(mid, k - 1, wr, wg, leader);
    }
  }
  return decision;
}

KnowledgeSet SERENADE::phi(int i, int k) {
  return _knowledge_sets[i][2 * k];
}

KnowledgeSet SERENADE::phi_opposite(int i, int k) {
  return _knowledge_sets[i][2 * k + 1];
}
/*
 * SERENA merge
 */
std::tuple<std::vector<int> /* decisions */,
           std::vector<int> /* which cycle each vertex is on */,
           std::vector<int> /* cycle lengths */,
           std::vector<std::pair<int, int>> /* weights */> SERENADE::run(const perm_t &sr,
                                                                const perm_t &sg,
                                                                const matrix_t &Q) {
  auto N = sg.size();
  std::vector<int> sg_inv(N, -1);
  for (int i = 0; i < N; ++i) {
    auto o = sg[i];
    assert(sg_inv[o] == -1);
    sg_inv[o] = i;
  }
  std::vector<int> decisions(N, -1);
  std::vector<int> cycles(N, -1);
  std::vector<int> cycle_lens;
  std::vector<int> cycle_dec;
  std::vector<std::pair<int, int> > cycle_weights;
  std::vector<bool> visited(N, false);

  for (int i = 0; i < N;) {
    auto wr = 0;
    auto wg = 0;
    auto o = -1;
    auto i_prime = -1;
    auto c = 0;
    auto c_id = cycle_lens.size();
    while (i < N && visited[i])
      ++i;
    if (i == N)
      break;
    auto i_old = i;
    do {
      visited[i] = true;
      cycles[i] = c_id;
      o = sr[i];
      i_prime = sg_inv[o];
      wr += Q[i][o];
      wg += Q[i_prime][o];
      ++c;
      i = i_prime;
    } while (!visited[i]);

    cycle_lens.push_back(c);
    cycle_dec.push_back((wr >= wg ? 1 : 0));
    cycle_weights.emplace_back(wr, wg);
    i = i_old;
  }

  for (int i = 0; i < N; ++i)
    decisions[i] = cycle_dec[cycles[i]];

  return {decisions, cycles, cycle_lens, cycle_weights};
}

void SERENADE::knowledge_disc(const matrix_t &Q, int max_iter, std::vector<int> &iterations) {
  for (int i = 0; i < _port_num; ++i) knowledge_disc_zero(i, Q);

  int cnt = 0;
  for (int k = 1; k <= max_iter && cnt < _port_num; ++k) {
    cnt = 0;
    for (int i = 0; i < _port_num; ++i) {
      if (_decisions[i] == -1) {
        knowledge_disc_send(i, k);
        ++iterations[i];
      } else {
        ++cnt;
      }
    }
    for (int i = 0; i < _port_num; ++i) {
      if (_decisions[i] == -1) {
        knowledge_disc_receive(i, k);
        knowledge_disc_halt_check(i, k);
      }
    }
  }

}
std::tuple<std::vector<int> /* decisions */,
           std::vector<int> /* # of iterations */,
           std::vector<int> /* cycle types */,
           int /* # of non-ouroboros */> SERENADE::approx(const SERENADE::perm_t &sr,
                                                          const SERENADE::perm_t &sg,
                                                          const SERENADE::matrix_t &Q) {
  init(sr, sg);

  std::vector<int> iterations(_port_num, 1);
  std::vector<int> type_of_cycles(_port_num, 0);

//  if (no_bs) {
//    knowledge_disc(Q, _port_num / 2, iterations);
//    return {_decisions, iterations, std::vector<int>(), 0};
//  } else
    knowledge_disc(Q, _max_iter, iterations);

  // binary search
  int n_cnt = 0;
  int k = _max_iter;

  for ( int i = 0;i < _port_num;++ i ) {
    if (_decisions[i] == -1) {
      auto ks = phi_opposite(i, k);
      auto l = ks.leader;
      if (i == l) {//
        auto ks_prime = phi(i, k);
        _decisions[l] = (ks_prime.wr >= ks_prime.wg ? 1 : 0);
        ++n_cnt;
      }
      type_of_cycles[i] = 1;
    }
  }

  for (int i = 0; i < _port_num; ++i) {
    if (_decisions[i] == -1) {
      auto ks = phi_opposite(i, k);
      auto l = ks.leader;
      _decisions[i] = _decisions[l];
    }
  }

  return {_decisions, iterations, type_of_cycles, n_cnt};

}
std::tuple<std::vector<int> /* decisions */,
           std::vector<int> /* which cycle each vertex is on */,
           std::vector<int> /* cycle lengths */,
           std::vector<std::pair<int, int>> /* weights */> SERENADE::run_approx(const SERENADE::perm_t &sr,
                                                                                const SERENADE::perm_t &sg,
                                                                                const SERENADE::matrix_t &Q,
                                                                                const std::vector<int> &ourobors) {
  auto N = sg.size();
  std::vector<int> sg_inv(N, -1);
  for (int i = 0; i < N; ++i) {
    auto o = sg[i];
    assert(sg_inv[o] == -1);
    sg_inv[o] = i;
  }
  std::vector<int> decisions(N, -1);
  std::vector<int> cycles(N, -1);
  std::vector<int> cycle_lens;
  std::vector<int> cycle_dec;
  std::vector<std::pair<int, int> > cycle_weights;
  std::vector<bool> visited(N, false);

  for (int i = 0; i < N;) {
    auto wr = 0;
    auto wg = 0;
    auto o = -1;
    auto i_prime = -1;
    auto c = 0;
    auto c_id = cycle_lens.size();
    while (i < N && visited[i])
      ++i;
    if (i == N)
      break;
    auto i_old = i;
    auto leader = i;
    do {
      visited[i] = true;
      cycles[i] = c_id;
      o = sr[i];
      i_prime = sg_inv[o];
      wr += Q[i][o];
      wg += Q[i_prime][o];
      ++c;
      i = i_prime;
      leader = std::min(i, leader);
    } while (!visited[i]);

    cycle_lens.push_back(c);
    cycle_weights.emplace_back(wr, wg);

    if (ourobors[cycle_lens.back()] == 0)
        cycle_dec.push_back((wr >= wg ? 1 : 0));
    else {
      auto cl = cycle_lens.back();
      auto tmp_i = leader;
      auto tmp_o = -1;
      auto tmp_i_prime = -1;
      while ( cl <= _port_num ) {
        tmp_o = sr[tmp_i];
        tmp_i_prime = sg_inv[tmp_o];
        wr += Q[tmp_i][tmp_o];
        wg += Q[tmp_i_prime][tmp_o];
        tmp_i = tmp_i_prime;
        ++ cl;
      }
      cycle_dec.push_back((wr >= wg ? 1 : 0));
    }

    i = i_old;
  }

  for (int i = 0; i < N; ++i)
    decisions[i] = cycle_dec[cycles[i]];

  return {decisions, cycles, cycle_lens, cycle_weights};
}

}
#endif //PROJECT_SERENADE_HPP
