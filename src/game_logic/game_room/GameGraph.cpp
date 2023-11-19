//
// Created by xPC on 18.11.2023.
//

#include "GameGraph.h"
#include <climits>
#include <stdexcept>

bool GameGraph::isValidEdge(unsigned int source, unsigned int destination) {
    // 1 node cycles are not allowed
    if (source == destination)
        return false;

    if (source >= GAME_NODE_NUM || destination >= GAME_NODE_NUM)
        return false;

    return true;
}

bool GameGraph::hasUndirectedEdge(unsigned int source, unsigned int destination) {
    if (!isValidEdge(source, destination))
        throw std::runtime_error("GameGraph::hasUndirectedEdge: ERROR - [" + std::to_string(source) + "," + std::to_string(destination)
                                 + "] is not a valid edge.");

    if (_adjacencyMatrix[source][destination] == 1 && _adjacencyMatrix[destination][source] == 1)
        return true;

    return false;
}

void GameGraph::addUndirectedEdge(unsigned int source, unsigned int destination) {
    _adjacencyMatrix[source][destination] = 1;
    _adjacencyMatrix[destination][source] = 1;
}

bool GameGraph::hasCycleOfLength3() {
    return findShortestCycle() == 3;
}

unsigned int GameGraph::_findShortestCycle(unsigned int node, int parent, int d, std::array<int, GAME_NODE_NUM> depth) {
    if (depth[node] != -1) {
        return d - depth[node];
    }
    depth[node] = d;

    unsigned int answer = INT_MAX;
    // for each neighbor of node
    for (int v = 0; v < GAME_NODE_NUM; v++) {
        // ignore parent and non-neighbors
        if (v != parent && _adjacencyMatrix[node][v] != 0) {
            answer = std::min(answer, _findShortestCycle(v, node, d + 1, depth));
        }
    }
    return answer;
}

unsigned int GameGraph::findShortestCycle() {
    std::array<int, GAME_NODE_NUM> depth{};
    depth.fill(-1);
    return _findShortestCycle(0, -1, 1, depth);
}

std::string GameGraph::serializeUndirectedEdges() {
    std::string result;
    for (int i = 0; i < GAME_NODE_NUM; ++i) {
        for (int j = 0; j < GAME_NODE_NUM; ++j) {
            if (j <= i)
                continue;
            if (!hasUndirectedEdge(i, j))
                continue;
            result += "{" + std::to_string(i) + "," + std::to_string(j) + "}";
            if (i == GAME_NODE_NUM - 2 && j == GAME_NODE_NUM - 1)
                continue;
            result += ",";
        }
    }
    return result;
}

void GameGraph::clear() {
    for (int i = 0; i < GAME_NODE_NUM; ++i) {
        for (int j = 0; j < GAME_NODE_NUM; ++j) {
            _adjacencyMatrix[i][j] = 0;
        }
    }
}