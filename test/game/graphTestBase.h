//
// Created by xPC on 08.11.2023.
//

#pragma once

#include "Game.h"
#include <gtest/gtest.h>
#include <thread>

class graphTestBase : public ::testing::Test {
  protected:
    GameGraph* g;
};
