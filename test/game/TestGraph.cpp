//
// Created by xPC on 08.11.2023.
//
#include "graphTestBase.h"
#include <gtest/gtest.h>

class TestGraph : public graphTestBase {
  protected:
    void SetUp() override
    {
        g = new GameGraph{};
    }

    void TearDown() override
    {
        delete g;
    }
};

TEST_F(TestGraph, Test_valid_edges)
{
    bool res = true;
    for (int i = 0; i < g->GAME_NODE_NUM; ++i) {
        for (int j = 0; j < g->GAME_NODE_NUM; ++j) {
            if (i == j)
                continue;
            res = res && g->isValidEdge(i, j);
            res = res && g->isValidEdge(j, i);
        }
    }

    ASSERT_TRUE(res);
}

TEST_F(TestGraph, Test_cycle_length1)
{
    g->addUndirectedEdge(0, 1);
    g->addUndirectedEdge(0, 2);
    g->addUndirectedEdge(0, 5);
    g->addUndirectedEdge(1, 5);
    g->addUndirectedEdge(2, 4);
    g->addUndirectedEdge(4, 5);

    int result = g->findShortestCycle();

    ASSERT_EQ(result, 3);
}

TEST_F(TestGraph, Test_cycle_length2)
{
    g->addUndirectedEdge(0, 1);
    g->addUndirectedEdge(0, 2);
    g->addUndirectedEdge(1, 5);
    g->addUndirectedEdge(2, 4);
    g->addUndirectedEdge(4, 5);

    int result = g->findShortestCycle();

    ASSERT_EQ(result, 5);
}

TEST_F(TestGraph, Test_cycle_length3)
{
    g->addUndirectedEdge(0, 1);
    g->addUndirectedEdge(0, 2);
    g->addUndirectedEdge(0, 3);
    g->addUndirectedEdge(0, 4);
    g->addUndirectedEdge(0, 5);
    g->addUndirectedEdge(1, 2);
    g->addUndirectedEdge(1, 3);
    g->addUndirectedEdge(1, 4);
    g->addUndirectedEdge(1, 5);
    g->addUndirectedEdge(2, 3);
    g->addUndirectedEdge(2, 4);
    g->addUndirectedEdge(2, 5);
    g->addUndirectedEdge(3, 4);
    g->addUndirectedEdge(3, 5);
    g->addUndirectedEdge(4, 5);

    int result = g->findShortestCycle();

    ASSERT_EQ(result, 3);
}

TEST_F(TestGraph, Test_cycle_length4)
{
    g->addUndirectedEdge(0, 1);
    g->addUndirectedEdge(0, 2);
    g->addUndirectedEdge(2, 4);
    g->addUndirectedEdge(4, 5);
    g->addUndirectedEdge(5, 3);
    g->addUndirectedEdge(3, 1);

    int result = g->findShortestCycle();

    ASSERT_EQ(result, 6);
}

TEST_F(TestGraph, Test_cycle_length5)
{
    int result = g->findShortestCycle();

    ASSERT_EQ(result, INT_MAX);
}

TEST_F(TestGraph, Test_edge_serialization1)
{
    g->addUndirectedEdge(0, 1);
    g->addUndirectedEdge(0, 2);
    g->addUndirectedEdge(0, 5);
    g->addUndirectedEdge(1, 5);
    g->addUndirectedEdge(2, 4);
    g->addUndirectedEdge(4, 5);

    std::string result = g->serializeUndirectedEdges();

    ASSERT_EQ(result, "{0,1},{0,2},{0,5},{1,5},{2,4},{4,5}");
}

TEST_F(TestGraph, Test_edge_serialization2)
{
    g->addUndirectedEdge(0, 1);
    g->addUndirectedEdge(0, 2);
    g->addUndirectedEdge(0, 3);
    g->addUndirectedEdge(0, 4);
    g->addUndirectedEdge(0, 5);
    g->addUndirectedEdge(1, 2);
    g->addUndirectedEdge(1, 3);
    g->addUndirectedEdge(1, 4);
    g->addUndirectedEdge(1, 5);
    g->addUndirectedEdge(2, 3);
    g->addUndirectedEdge(2, 4);
    g->addUndirectedEdge(2, 5);
    g->addUndirectedEdge(3, 4);
    g->addUndirectedEdge(3, 5);
    g->addUndirectedEdge(4, 5);

    std::string result = g->serializeUndirectedEdges();

    ASSERT_EQ(result, "{0,1},{0,2},{0,3},{0,4},{0,5},{1,2},{1,3},{1,4},{1,5},{2,3},{2,4},{2,5},{3,4},{3,5},{4,5}");
}

TEST_F(TestGraph, Test_edge_serialization3)
{
    g->addUndirectedEdge(0, 1);
    g->addUndirectedEdge(0, 2);
    g->addUndirectedEdge(2, 4);
    g->addUndirectedEdge(4, 5);
    g->addUndirectedEdge(5, 3);
    g->addUndirectedEdge(3, 1);

    std::string result = g->serializeUndirectedEdges();

    ASSERT_EQ(result, "{0,1},{0,2},{1,3},{2,4},{3,5},{4,5}");
}