#ifndef DIGRAPH_H
#define DIGRAPH_H

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>

// An intersection between edges in a directed graph.
// (Aliased to improve readability)
using vertex = std::string;

// A weighted edge that connects two vertices
// on a directed graph.
struct edge {
  vertex src;
  vertex dest;
  int weight;
};

using weighted_path = std::pair<vertex, int>;

// A representation of a weighted directed graph.
class digraph {
 public:
  digraph(const std::vector<edge> &edges);

  using adjacency_map = std::unordered_map<vertex, std::vector<weighted_path>>;

  adjacency_map::const_iterator begin() const { return std::begin(adjacency_); }
  adjacency_map::const_iterator end() const { return std::end(adjacency_); }

  const std::vector<weighted_path> &operator[](const vertex &v) const
  {
    return adjacency_.at(v);
  }

  const std::unordered_set<vertex> &vertices() const { return vertices_; };

 private:
  adjacency_map adjacency_;
  std::unordered_set<vertex> vertices_;
};

#endif