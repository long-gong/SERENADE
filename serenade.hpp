
#ifndef PROJECT_SERENADE_HPP
#define PROJECT_SERENADE_HPP
#include <vector>
#include <fstream>


namespace saber {
struct KnowledgeSet {
  int id;
  int wr;
  int wg;
  int leader;
  KnowledgeSet(int id, int wr, int wg, int leader) : id(id), wr(wr), wg(wg), leader(leader){}
  KnowledgeSet(int id, int wr, int wg) : id(id), wr(wr), wg(wg), leader(-1){}
};

class SERENADE {
 public:
  typedef perm_t std::vector<int>;
  typedef matrix_t std::vector<std::vector<int> >;
 protected:
  perm_t& _sigma_r;// random mat
  perm_t& _sigma_g;//
  int _max_iter;
  int port_num;
  perm_t _sigma;
  per_t _sigma_inv;
  perm_t decisions;
  std::vector<std::vector< KnowledgeSet > > knowledge_sets;
  std::vector<std::vector< KnowledgeSet* > > B;
  std::vector<std::vector<KnowledgeSet> > receive;
 public:
  std::pair<std::vector<int>, int>> emulate(perm_t &sr, perm_t &sg, matrix_t &Q);
 private:
  void knowledge_disc_zero(int i);
  void knowledge_disc_send(int i, int k);
  void knowledge_disc_receive(int i, int k);
  void knowledge_disc_halt_check(int i, int k);
  void binary_search(int i, int k, int wr, int wg, const int leader);
  KnowledgeSet& phi(int i, int k) ;
  KnowledgeSet& phi_opposite(int i, int k);

};

std::pair<std::vector<int>, int>>  SERENADE::emulate(perm_t &sr, perm_t &sg, matrix_t &Q) {

  for ( int i = 0 ;i < port_num;++ i ) {
    knowledge_discovery_zero(i, Q);
  }

  for ( int k = 1;k <= max_iter;++ k ) {
    for ( int i = 0 ;i < port_num;++ i ) {
      if ( decisions[i] == -1 ) {
        knowledge_discovery_send(i, k);
        knowledge_discovery_receive(i, k);
        knowledge_disc_halt_check(i, k);
      }
    }
  }
}

/*
 * The 0th iteration of SERENADE
 */
void SERENADE::knowledge_disc_zero(int i, matrix_t &Q) {
  auto i_d = sigma[i];
  auto i_u = sigma_inverse[i];
  knowledge_sets[i].emplace_back(i_d, Q[i][r_matching[i]], Q[i_d][r_matching[i]], -1);
  knowledge_sets[i].emplace_back(i_d, Q[i_u][r_matching[i_u]], Q[i][r_matching[i_u]], std::min(i_u, i));
  // check whether i has discovered some vertex twice
  if ( i_d == i ) {
    // cycle with size of 1
    decisions[i] = (knowledge_sets[i].back().wr >= knowledge_sets[i].back().wg ? 1:0);
    return ;
  } else if ( i_d == i_u ) {
    // cycle with size of 2
    auto phi_k = phi(i, 0);
    auto phi_op_k = phi_opposite(i, 0);
    decisions[i] = (phi_k.wr + phi_op_k.wr >= phi_k.wg + phi_op_k.wg ? 1:0);
    return ;
  }
  return ;
}
/*
 * The k^{th} iteration: sending
 */
void SERENADE::knowledge_disc_send(int i, int k) {
  auto ks = phi(i, k - 1);
  auto ks_op  = phi_opposite(i, k - 1);
  auto i_d = ks.id;
  auto i_u = ks_op.id;
  receive[i_d].push_back(ks_op);
  receive[i_u].push_back(ks);
}

/*
 * The k^{th} iteration: receiving
 */
void SERENADE::knowledge_disc_receive(int i, int k) {
  auto ks_local = phi(i, k - 1);
  auto ks_local_op = phi_opposite(i, k - 1);

  auto ks_from_d = (receive[i][0].leader == -1) ? receive[i][0] : receive[i][1];
  auto ks_from_u = (receive[i][0].leader != -1) ? receive[i][0] : receive[i][1];

  // calculate the new knowledge sets
  knowledge_sets[i].emplace_back(ks_from_d.id, ks.wr + ks_local.wr, ks_local.wg + ks_from_d.wg, -1);
  knowledge_sets[i].emplace_back(ks_from_u.id, ks_from_u.wr + ks_local_op.wr,
                                 ks_local_op.wg + ks_from_u.wg, std::min(ks_from_u.leader, ks_local_op.leader));
}

/*
 * The k^{th} iteration: halt checking
 */
void SERENADE::knowledge_disc_halt_check(int i, int k) {
  auto &ks1 = phi(i, k);
  auto &ks2 = phi_opposite(i, k);

  auto i_d = ks1.id;
  auto i_u = ks2.id;

  if ( B[i_d] != nullptr ) {
    // rediscover
    if ( B[i_d]->leader == -1 ) {
      // same direction
      decisions[i] = ((ks1.wr - B[i_d]->wr) > (ks1.wg - B[i_d]->wg) ? 1 : 0);
    } else {
      // opposite direction
      decisions[i] = ((ks1.wr + B[i_d]->wr) > (ks1.wg + B[i_d]->wg) ? 1 : 0);
    }
    return ;
  }
  if ( B[i_u] != nullptr ) {
    // rediscover
    if ( B[i_u]->leader != -1 ) {
      // same direction
      decisions[i] = ((ks2.wr - B[i_u]->wr) > (ks2.wg - B[i_u]->wg) ? 1 : 0);
    } else {
      // opposite direction
      decisions[i] = ((ks2.wr + B[i_u]->wr) > (ks2.wg + B[i_u]->wg) ? 1 : 0);
    }
    return ;
  }
  if ( i_d == i_u ) {
    decisions[i] = ((ks1.wr + ks2.wr) > (ks1.wg + ks2.wg) ? 1 : 0);
    return ;
  }

  B[i_d] = &ks1;
  B[i_u] = &ks2;

  receive[i].clear();
}

void SERENADE::binary_search(int i, int k, int wr, int wg, const int leader) {

}

KnowledgeSet &SERENADE::phi(int i, int k) {
  return knowledge_sets[i][2 * k];
}

KnowledgeSet &SERENADE::phi_opposite(int i, int k) {
  return knowledge_sets[i][2 * k + 1];
}

}
#endif //PROJECT_SERENADE_HPP
