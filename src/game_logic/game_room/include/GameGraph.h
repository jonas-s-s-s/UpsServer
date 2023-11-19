//
// Created by xPC on 18.11.2023.
//

#pragma once

#include <array>
#include <string>
class GameGraph {
  public:
    constexpr static unsigned int GAME_NODE_NUM = 6;

    bool isValidEdge(unsigned int source, unsigned int destination);

    bool hasUndirectedEdge(unsigned int source, unsigned int destination);

    void addUndirectedEdge(unsigned int source, unsigned int destination);

    bool hasCycleOfLength3();

    unsigned int findShortestCycle();

    std::string serializeUndirectedEdges();

    void clear();

  private:
    unsigned int _findShortestCycle(unsigned int node, int parent, int d, std::array<int, GAME_NODE_NUM> depth);

    unsigned int _adjacencyMatrix[GAME_NODE_NUM][GAME_NODE_NUM]{};
};