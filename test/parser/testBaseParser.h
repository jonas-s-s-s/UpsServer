//
// Created by xPC on 08.11.2023.
//

#pragma once

#include <gtest/gtest.h>
#include "BufferedParser.h"


class testBaseParser : public ::testing::Test {
protected:
    BufferedParser* bufferedParser;
};
