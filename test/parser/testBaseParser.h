//
// Created by xPC on 08.11.2023.
//

#pragma once

#include "BufferedParser.h"
#include <gtest/gtest.h>

class testBaseParser : public ::testing::Test {
  protected:
    BufferedParser* bufferedParser;
};
