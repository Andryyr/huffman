#include <fstream>
#include "huffman.h"

void help() {
    std::cout << "Please write: (-e | -d) source target" << std::endl;
    exit(0);
}

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        help();
    }
    std::string option = std::string(argv[1]);
    std::string source = argv[2];
    std::string target = argv[3];

    std::ifstream istrm(source, std::ifstream::binary);
    std::ofstream ostrm(target, std::ofstream::binary);
    if (!istrm.is_open() || !ostrm.is_open())
    {
        std::cout << "File opening error" << std::endl;
        return 0;
    }

    if (option == "-e")
        huffman::encode(istrm, ostrm);
    else if (option == "-d")
    {
        if (!huffman::decode(istrm, ostrm))
        {
            std::cout << "File corrupted" << std::endl;
            return 0;
        }
    } else {
        help();
    }
}