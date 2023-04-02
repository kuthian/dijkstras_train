#include "dijkstra.h"

#include <limits>
#include <queue>
#include <stdexcept>

void dijkstra_calculator::calculate(const digraph& G, const vertex& S)
{
  distances_.clear();

  std::map<vertex, vertex> parents;

  for (const vertex& V : G.vertices()) {
    distances_[V] = std::numeric_limits<int>::max();
  }
  distances_[S] = 0;

  auto ascending = [](const weighted_path& lhs, const weighted_path& rhs) {
    return lhs.second > rhs.second;
  };
  std::priority_queue<weighted_path, std::vector<weighted_path>, decltype(ascending)>
      Q{ascending};

  Q.push({S, 0});

  while (!Q.empty()) {
    const vertex U = Q.top().first;
    Q.pop();
    try {
      for (const auto& V_pair : G[U]) {
        const vertex& V = V_pair.first;
        int U_V_weight = V_pair.second;
        int d = distances_[U] + U_V_weight;
        if (distances_[V] > d) {
          parents[V] = U;
          distances_[V] = d;
          Q.emplace(V, distances_[V]);
        }
      }
    }
    // vertex with no paths 
    catch (const std::out_of_range&) { continue; }
  }

  shortest_paths_.clear();

  for (const vertex& dest : G.vertices()) {
    const auto src_itr = parents.find(S);
    auto& path = shortest_paths_[dest];
    auto itr = parents.find(dest);
    path.push_front(dest);
    while (itr != src_itr) {
      path.push_front(itr->second);
      itr = parents.find(itr->second);
    }
  }
}