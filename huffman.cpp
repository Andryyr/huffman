//
// Created by andry on 27.09.2018.
//

#include <fstream>
#include <set>
#include <unordered_map>
#include "huffman.h"

struct huffman::Node
        : public std::initializer_list<::huffman::Node> {
    char symb;
    uint64_t weight;
    bool single;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;

    Node(char symb,
            uint64_t weight,
            bool single = true,
            std::unique_ptr<Node> left = nullptr,
            std::unique_ptr<Node> right = nullptr):
            symb(symb),
            weight(weight),
            single(single),
            left(std::move(left)),
            right(std::move(right))
    {}
};

void huffman::encode(std::istream &fin, std::ostream &fout)
{
    std::array<uint64_t, 256> freq_array = {};

    char c;
    while(fin.peek() != std::ifstream::traits_type::eof())
    {
        fin.read(&c, sizeof(char));
        freq_array[static_cast<unsigned char>(c)]++;
    }

    std::map<char, uint64_t> freq;
    freq['a'] = freq['b'] = 0;

    for (uint32_t i = 0; i != 256; ++i)
    {
        uint64_t fr = freq_array[static_cast<unsigned char>(i)];
        if (fr != 0)
            freq[i] = fr;
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

    std::array<std::vector<bool>, 256> codes;
    std::vector<bool> curr_code;
    {
        std::unique_ptr<Node> root = build_tree(freq);
        gen_codes(*root, codes, curr_code);
    }

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

    std::unique_ptr<Node> root = build_tree(freq);

    char buffer[buf_size];
    char numb_of_bits = 8;
    Node* node = root.get();

    while(fin)
    {
        fin.read(buffer, buf_size * sizeof(char));
        auto symb_count = size_t(fin.gcount());
        if (symb_count == 0)
            break;
        for(size_t i = 0; i < symb_count - 1; i++)
        {
            for(size_t j = 0; j < size_t(numb_of_bits); j++)
            {
                node = (buffer[i] >> j) & 1 ? node->right.get() : node->left.get();
                if (!node)
                    return false;

                if (node->single)
                {
                    fout.write(&node->symb, sizeof(char));
                    node = root.get();
                }
            }
        }

        if (!fin)
            numb_of_bits = numb_of_bits - fake_zero;
        for(size_t j = 0; j < size_t(numb_of_bits); j++)
        {
            node = (buffer[symb_count - 1] >> j) & 1 ? node->right.get() : node->left.get();
            if (!node)
                return false;

            if (node->single)
            {
                fout.write(&node->symb, sizeof(char));
                node = root.get();
            }
        }
    }

    return true;
}

void huffman::gen_codes(huffman::Node& v, std::array<std::vector<bool>, 256>& codes, std::vector<bool>& curr_code)
{
    if (v.single)
    {
        codes[static_cast<unsigned char>(v.symb)] = curr_code;
        curr_code.pop_back();
        return;
    }

    curr_code.push_back(false);
    gen_codes(*v.left, codes, curr_code);
    curr_code.push_back(true);
    gen_codes(*v.right, codes, curr_code);

    if (!curr_code.empty())
        curr_code.pop_back();
}

std::unique_ptr<huffman::Node> huffman::build_tree(std::map<char, uint64_t> &freq)
{
    std::multimap<uint64_t, std::unique_ptr<Node>> nodes;
    for(auto& i : freq)
    {
        nodes.insert({i.second, std::make_unique<Node>(i.first, i.second)});
    }

    while(nodes.size() > 1)
    {
        std::unique_ptr<Node> a = std::move(nodes.begin()->second);
        nodes.erase(nodes.begin());
        std::unique_ptr<Node> b = std::move(nodes.begin()->second);
        nodes.erase(nodes.begin());

        uint64_t w = a->weight + b->weight;
        nodes.insert({w, std::make_unique<Node>('0', w, false, std::move(a), std::move(b))});
    }
    return std::move(nodes.begin()->second);
}
