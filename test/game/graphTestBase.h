//
// Created by xPC on 08.11.2023.
//

#pragma once

#include <gtest/gtest.h>
#include <thread>
#include "Game.h"

class graphTestBase : public ::testing::Test {
protected:
    GameGraph* g;
};
