#include <iostream>
#include "../digraph.h"
#include "../dijkstra.h"

int main()
{
  std::vector<edge> edges = { 
    { "S", "b", 10 },                              
    { "S", "c", 20 },                              
    { "c", "d", 20 },                              
    { "c", "e", 33 },                              
    { "b", "e", 10 },                              
    { "b", "d", 50 },                              
    { "d", "e", 20 },                              
    { "e", "f", 1 },                              
    { "d", "f", 2 },                              
  };

  digraph G(edges);

  std::cout << "--Digraph--\n";
  for (const auto& v : G) {
    for (const auto& adjacent_v : v.second) {
      std::cout << v.first << "->" << adjacent_v.first << ": " << adjacent_v.second << std::endl;
    }
  }

  dijkstra_calculator dijkstra;
  dijkstra.calculate(G, {"S"});

  std::cout << "--Distances--\n";
  for( const auto& d : dijkstra.distances() ) {
    std::cout << "S" << "->" << d.first << ": " << d.second << std:: endl;
  }
  std::cout << "------------\n";

  // }
  return 0;
}