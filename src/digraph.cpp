#include "digraph.h"

digraph::digraph(const std::vector<edge>& edges)
{
  for (const auto& edge : edges) {
    adjacency_[edge.src].emplace_back(edge.dest, edge.weight);
    vertices_.insert(edge.src);
    vertices_.insert(edge.dest);
  }
}