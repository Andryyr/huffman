//
// Created by andry on 27.09.2018.
//

#ifndef HUFFMAN_V2_HUFFMAN_H
#define HUFFMAN_V2_HUFFMAN_H

#include <vector>
#include <iostream>
#include <map>

class huffman {
public:
    static void encode(std::istream& fin, std::ostream& fout);
    static bool decode(std::istream& fin, std::ostream& fout);

private:
    struct Node;

    static void gen_codes(Node* v, std::map<char, std::vector<bool>>& codes, std::vector<bool>& curr_code);

    static Node* build_tree(std::map<char, uint64_t>& freq);

    static void delete_tree(Node* root);

    static const uint32_t buf_size = 1024 * 1024;
};


#endif //HUFFMAN_V2_HUFFMAN_H
