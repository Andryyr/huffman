//
// Created by andry on 29.09.2018.
//

#include <iostream>

#include "gtest/gtest.h"
#include "huffman.h"


TEST(correctness, empty) {
    std::stringstream in("");
    std::stringstream c;
    std::stringstream d;

    huffman::encode(in, c);
    huffman::decode(c, d);

    EXPECT_EQ(in.str(), d.str());
}

TEST(correctness, one_symbol) {
    std::stringstream in("a");
    std::stringstream c;
    std::stringstream d;

    huffman::encode(in, c);
    huffman::decode(c, d);

    EXPECT_EQ(in.str(), d.str());
}

TEST(correctness, all_char) {
    std::stringstream in;
    std::stringstream c;
    std::stringstream d;

    for (int i = -128; i <= 127; i++) {
        in << char(i);
    }

    huffman::encode(in, c);
    huffman::decode(c, d);

    EXPECT_EQ(in.str(), d.str());
}

TEST(correctness, rand_big) {
    std::stringstream in;
    std::stringstream c;
    std::stringstream d;

    for (int i = 0; i < int(1e7); i++) {
        in << (char(rand() % 256));
    }

    huffman::encode(in, c);
    huffman::decode(c, d);

    EXPECT_EQ(in.str(), d.str());
}

TEST(correctness, invalid_file) {
    std::stringstream c;
    std::stringstream d("");

    c << "abacabadaaba";

    EXPECT_EQ(false, huffman::decode(c, d));
}

TEST(correctness, null_string) {
    std::stringstream in;
    std::stringstream c;
    std::stringstream d;

    for (int i = 0; i < 100; i++) {
        in << char(0);
    }

    huffman::encode(in, c);
    huffman::decode(c, d);

    EXPECT_EQ(in.str(), d.str());
}

TEST(correctness, similar_chars) {
    std::stringstream in;
    std::stringstream c;
    std::stringstream d;

    for (int i = 0; i < 10000; i++) {
        in << 'a';
    }

    huffman::encode(in, c);
    huffman::decode(c, d);
    EXPECT_EQ(in.str(), d.str());
}
