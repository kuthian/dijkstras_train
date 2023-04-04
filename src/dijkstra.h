#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <deque>

#include "digraph.h"

class dijkstra_calculator {
 public:
  /**
   * Find the shortest paths within a weighted directed graph
   * using djisktra's algorithm, given a starting position.
   *
   * @param G Weighted directed graph.
   * @param S The starting position.
   */
  void calculate(const digraph& G, const vertex& S);

  // @return Distances to all vertices from vertex S.
  const std::unordered_map<vertex, int>& distances() const { return distances_; }
  // @return Shortest paths to all vertices from vertex S.
  const std::unordered_map <vertex, std::deque<vertex>>& shortest_paths() const { return shortest_paths_; }

 private:
  std::unordered_map<vertex, int> distances_;
  std::unordered_map<vertex, std::deque<vertex> > shortest_paths_;
};

#endif