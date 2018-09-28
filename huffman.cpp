//
// Created by andry on 27.09.2018.
//

#include <fstream>
#include <set>
#include <unordered_set>
#include "huffman.h"

struct huffman::Node
        : public std::initializer_list<::huffman::Node> {
    char symb;
    uint64_t weight;
    bool single;
    Node* left;
    Node* right;

    Node(char symb,
            uint64_t weight,
            bool single = true,
            Node* left = nullptr,
            Node* right = nullptr):
            symb(symb),
            weight(weight),
            single(single),
            left(left),
            right(right) {}
};

void huffman::encode(std::istream &fin, std::ostream &fout)
{
    std::map<char, uint64_t> freq;
    char c;
    freq['a'] = freq['b'] = 0;
    while(fin.peek() != std::ifstream::traits_type::eof())
    {
        fin.read(&c, sizeof(char));
        freq[c]++;
    }
    fin.seekg(fin.beg);
    char buffer[buf_size];
    fout.write(&buffer[0], sizeof(char));

    uint16_t numb_of_symb = static_cast<uint16_t >(freq.size());
    fout.write(reinterpret_cast<const char *>(&numb_of_symb), sizeof(numb_of_symb));
    for (const auto i: freq)
    {
        char key = i.first;
        uint64_t count = i.second;
        fout.write(&key, sizeof(key));
        fout.write(reinterpret_cast<const char *>(&count), sizeof(count));
    }

    Node* root = build_tree(freq);
    std::map<char, std::vector<bool>> codes;
    std::vector<bool> curr_code;
    gen_codes(root, codes, curr_code);

    char actual_code = 0;
    char bits_counter = 0;
    char numb_of_bits = 8;

    while(fin)
    {
        fin.read(buffer, buf_size * sizeof(char));
        auto numb_of_symbs = size_t(fin.gcount());
        for(size_t i = 0; i < numb_of_symbs; i++)
        {
            std::vector<bool> symb_code = codes[buffer[i]];
            for (const auto next : symb_code)
            {
                actual_code |= (next << bits_counter++);
                if (bits_counter == numb_of_bits)
                {
                    fout.write(&actual_code, sizeof(actual_code));
                    actual_code = 0;
                    bits_counter = 0;
                }
            }
        }
    }

    if (bits_counter)
    {
        bits_counter = numb_of_bits - bits_counter;
        fout.write(&actual_code, sizeof(actual_code));
    }
    fout.seekp(0);
    fout.write(&bits_counter, sizeof(bits_counter));
}

bool huffman::decode(std::istream &fin, std::ostream &fout)
{
    char fake_zero;
    fin.read(&fake_zero, sizeof(fake_zero));

    if (!fin)
        return false;
    std::map<char, uint64_t> freq;
    uint16_t numb_of_symb;
    fin.read(reinterpret_cast<char *>(&numb_of_symb), sizeof(numb_of_symb));
    for(size_t i = 0; i < numb_of_symb; i++)
    {
        char key;
        uint64_t count;
        fin.read(&key, sizeof(key));
        if (!fin)
            return false;
        fin.read(reinterpret_cast<char *>(&count), sizeof(count));
        if (freq.find(key) != freq.end())
            return false;
        freq[key] = count;
    }

    Node* root = build_tree(freq);

    char buffer[buf_size];
    char numb_of_bits = 8;
    Node* node = root;

    while(fin)
    {
        fin.read(buffer, buf_size * sizeof(char));
        auto symb_count = size_t(fin.gcount());
        if (symb_count == 0)
            break;
        for(size_t i = 0; i < symb_count - 1; i++)
        {
            for(size_t j = 0; j < numb_of_bits; j++)
            {
                node = (buffer[i] >> j) & 1 ? node->right : node->left;
                if (!node)
                    return false;
                if (node->single)
                {
                    fout.write(&node->symb, sizeof(char));
                    node = root;
                }
            }
        }

        if (!fin)
            numb_of_bits = numb_of_bits - fake_zero;
        for(size_t j = 0; j < numb_of_bits; j++)
        {
            node = (buffer[symb_count - 1] >> j) & 1 ? node->right : node->left;
            if (!node)
                return false;
            if (node->single)
            {
                fout.write(&node->symb, sizeof(char));
                node = root;
            }
        }
    }

    return true;
}

void huffman::gen_codes(huffman::Node* v, std::map<char, std::vector<bool>>& codes, std::vector<bool>& curr_code)
{
    if (v->single)
    {
        codes[v->symb] = std::vector<bool>(curr_code);
        curr_code.pop_back();
        return;
    }

    curr_code.push_back(false);
    gen_codes(v->left, codes, curr_code);
    curr_code.push_back(true);
    gen_codes(v->right, codes, curr_code);

    if (!curr_code.empty())
        curr_code.pop_back();
}

huffman::Node *huffman::build_tree(std::map<char, uint64_t> &freq)
{
    auto cmp = [](const auto& lhs, const auto& rhs)
    {
        if (lhs->weight == rhs->weight)
            return true;
        return lhs->weight < rhs->weight;
    };
    std::set<Node*, decltype(cmp)>nodes(cmp);
    for(auto& i : freq)
    {
        nodes.insert(new Node(i.first, i.second));
    }

    while(nodes.size() > 1)
    {
        Node* a = *nodes.begin();
        nodes.erase(nodes.begin());
        Node* b = *nodes.begin();
        nodes.erase(nodes.begin());

        nodes.insert(new Node('0', a->weight + b->weight, false, a, b));
    }
    return *nodes.begin();
}
