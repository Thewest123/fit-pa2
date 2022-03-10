#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>
#include <set>
#include <queue>
#include <memory>
#include <functional>
#include <stdexcept>
using namespace std;
#endif /* __PROGTEST__ */

class CBitReader
{
private:
    vector<char> m_bytes;

public:
    long m_bitIndex = 0;
    long m_bitsCount = 0;

    CBitReader(ifstream &ifs)
    {
        char c;
        while (ifs.good() && ifs.get(c))
        {
            m_bytes.push_back(c);
        }

        m_bitsCount = m_bytes.size() * 8L;
    }

    /**
     * @brief Reads next bit
     *
     * @param[out] bit The next bit
     * @return true if the bit was read correctly
     * @return false if there are no more bits to read (EOF)
     */
    bool nextBit(bool &bit)
    {
        if (m_bitIndex >= m_bitsCount)
        {
            return false;
        }

        if ((size_t)(m_bitIndex / 8) >= m_bytes.size())
        {
            throw runtime_error("Can't read next bit!");
            return false;
        }

        int shiftCount = 7 - (m_bitIndex & 0b00000111);
        char lastBit = (m_bytes[m_bitIndex / 8] >> shiftCount) & 0b00000001;

        m_bitIndex++;

        bit = (lastBit == '\0') ? false : true;
        // cout << ((bit) ? '1' : '0');

        return true;
    }

    /**
     * @brief Reads next 8 bits and returns them as a char
     *
     * @return char
     */
    unsigned long nextChar()
    {
        bool bit = false;
        nextBit(bit);

        // Read ASCII
        if (!bit)
        {
            unsigned char builtChar = 0;
            for (int i = 6; i >= 0; i--)
            {
                bool bit = false;
                nextBit(bit);

                // Set i-th bit to 1
                if (bit)
                    builtChar |= 1 << i;
                // Clear i-th bit to 0
                else
                    builtChar &= ~(1 << i);
            }

            return builtChar;
        }

        // Read UTF-8
        int utfBytesCount = 0;
        unsigned long builtCharUtf = 0UL;

        while (bit)
        {
            utfBytesCount++;
            nextBit(bit);
        }

        if (utfBytesCount == 2)
        {
            builtCharUtf |= (1UL << 15);
            builtCharUtf |= (1UL << 14);
            builtCharUtf &= (~(1UL << 13));

            for (int i = 12; i >= 0; i--)
            {
                nextBit(bit);

                // Set i-th bit to 1
                if (bit)
                    builtCharUtf |= (1UL << i);
                // Clear i-th bit to 0
                else
                    builtCharUtf &= (~(1UL << i));

                if (((i == 7) && (!bit)) ||
                    ((i == 6) && (bit)))
                {
                    throw runtime_error("Invalid UTF-8 format (2 bytes)");
                }
            }

            return builtCharUtf;
        }

        if (utfBytesCount == 3)
        {
            builtCharUtf |= (1UL << 23);
            builtCharUtf |= (1UL << 22);
            builtCharUtf |= (1UL << 21);
            builtCharUtf &= (~(1UL << 20));

            for (int i = 19; i >= 0; i--)
            {
                nextBit(bit);

                // Set i-th bit to 1
                if (bit)
                    builtCharUtf |= (1UL << i);
                // Clear i-th bit to 0
                else
                    builtCharUtf &= (~(1UL << i));

                if (((i == 7 || i == 15) && (!bit)) ||
                    ((i == 6 || i == 14) && (bit)))
                {
                    throw runtime_error("Invalid UTF-8 format (3 bytes)");
                }
            }

            return builtCharUtf;
        }

        if (utfBytesCount == 4)
        {
            builtCharUtf |= (1UL << 31);
            builtCharUtf |= (1UL << 30);
            builtCharUtf |= (1UL << 29);
            builtCharUtf |= (1UL << 28);
            builtCharUtf &= (~(1UL << 27));

            for (int i = 26; i >= 0; i--)
            {
                nextBit(bit);

                // Set i-th bit to 1
                if (bit)
                    builtCharUtf |= (1UL << i);
                // Clear i-th bit to 0
                else
                    builtCharUtf &= (~(1UL << i));

                if (((i == 7 || i == 15 || i == 23) && (!bit)) ||
                    ((i == 6 || i == 14 || i == 22) && (bit)))
                {
                    throw runtime_error("Invalid UTF-8 format (4 bytes)");
                }
            }

            return builtCharUtf;
        }

        throw runtime_error("Bad ASCII input!");
    }

    /**
     * @brief Reads next 12 bits and returns them as a number
     *
     * @return int chunkSize
     */
    int nextChunkSize()
    {
        int chunkSize = 0;
        for (int i = 11; i >= 0; i--)
        {
            bool bit = false;
            nextBit(bit);

            // Set i-th bit to 1
            if (bit)
                chunkSize |= 1 << i;
            // Clear i-th bit to 0
            else
                chunkSize &= ~(1 << i);
        }

        return chunkSize;
    }
};

struct TNode
{
    TNode *left;
    TNode *right;
    unsigned long value;
    bool isLeaf;

    TNode(unsigned long value)
    {
        left = nullptr;
        right = nullptr;
        this->value = value;
        this->isLeaf = true;
    }

    TNode()
    {
        left = nullptr;
        right = nullptr;
        this->value = 0;
        this->isLeaf = false;
    }
};

TNode *buildTree(CBitReader &br)
{
    bool bit = false;
    br.nextBit(bit);

    // If bit was 0, we are building an inner node
    if (!bit)
    {
        TNode *node = new TNode();
        node->left = buildTree(br);
        node->right = buildTree(br);

        return node;
    }

    // Else we are building a leaf node
    unsigned long znak = br.nextChar();
    return new TNode(znak);
}

void deleteTree(TNode *node)
{
    if (node == nullptr)
        return;

    deleteTree(node->left);
    deleteTree(node->right);

    delete node;
}

bool decompressFile(const char *inFileName, const char *outFileName)
{

    ifstream ifs(inFileName, ios::in | ios::binary);
    ofstream ofs(outFileName, ios::out | ios::binary);

    if (!ifs)
    {
        ifs.close();
        ofs.close();

        return false;
    }

    CBitReader bitReader(ifs);

    // Build binary tree
    TNode *root = nullptr;

    try
    {
        root = buildTree(bitReader);

        bool bit;
        while (bitReader.nextBit(bit))
        {
            // Read chunk size
            int chunkSize = 4096;
            if (!bit)
                chunkSize = bitReader.nextChunkSize();

            // Read coded values and write to ofstream ofs
            for (int i = 0; i < chunkSize; i++)
            {
                TNode *finalNode = root;
                bool canRead;
                while ((canRead = bitReader.nextBit(bit)))
                {
                    if (bit)
                        finalNode = finalNode->right;
                    else
                        finalNode = finalNode->left;

                    if (finalNode->isLeaf)
                    {
                        unsigned long x = finalNode->value;

                        // Inspired from https://stackoverflow.com/a/3269948
                        char byte3 = (char)((0xff000000 & x) >> 24);
                        char byte2 = (char)((0x00ff0000 & x) >> 16);
                        char byte1 = (char)((0x0000ff00 & x) >> 8);
                        char byte0 = (char)(0x000000ff & x);

                        if (byte3 != '\0')
                            ofs << byte3;

                        if (byte2 != '\0')
                            ofs << byte2;

                        if (byte1 != '\0')
                            ofs << byte1;

                        if (byte0 != '\0')
                            ofs << byte0;

                        break;
                    }
                }

                // If chunkSize in file is wrong (bigger then real chunk size) or can't read more bits
                if (!canRead)
                {
                    ifs.close();
                    ofs.close();

                    deleteTree(root);

                    return false;
                }
            }
        }
    }
    catch (const runtime_error &e)
    {
        ifs.close();
        ofs.close();

        deleteTree(root);

        cout << e.what() << endl;

        return false;
    }

    ifs.close();

    ofs << flush;
    ofs.close();

    deleteTree(root);

    return true;
}

bool compressFile(const char *inFileName, const char *outFileName)
{
    // keep this dummy implementation (no bonus) or implement the compression (bonus)
    return false;
}
#ifndef __PROGTEST__
bool identicalFiles(const char *fileName1, const char *fileName2)
{
    ifstream ifs1(fileName1, ios::in | ios::binary);
    ifstream ifs2(fileName2, ios::in | ios::binary);

    // Set cursor to the end of files
    ifs1.seekg(0, ios_base::end);
    ifs2.seekg(0, ios_base::end);

    // Check files are same size
    if (ifs1.tellg() != ifs2.tellg())
        return false;

    // Set cursor to the beggining
    ifs1.seekg(0, ios_base::beg);
    ifs2.seekg(0, ios_base::beg);

    // Set iterators
    istreambuf_iterator<char> iterator1(ifs1);
    istreambuf_iterator<char> iterator2(ifs2);

    bool isEqual = equal(iterator1, istreambuf_iterator<char>(), iterator2);

    // Close stream
    ifs1.close();
    ifs2.close();

    return isEqual;
}

int main(void)
{
    assert(!decompressFile("tests/wrong_ascii.huf", "tempfile"));

    assert(decompressFile("tests/test0.huf", "tempfile"));
    assert(identicalFiles("tests/test0.orig", "tempfile"));

    assert(decompressFile("tests/test1.huf", "tempfile"));
    assert(identicalFiles("tests/test1.orig", "tempfile"));

    assert(decompressFile("tests/test2.huf", "tempfile"));
    assert(identicalFiles("tests/test2.orig", "tempfile"));

    assert(decompressFile("tests/test3.huf", "tempfile"));
    assert(identicalFiles("tests/test3.orig", "tempfile"));

    assert(decompressFile("tests/test4.huf", "tempfile"));
    assert(identicalFiles("tests/test4.orig", "tempfile"));

    assert(!decompressFile("tests/test5.huf", "tempfile"));

    assert(decompressFile("tests/extra0.huf", "tempfile"));
    assert(identicalFiles("tests/extra0.orig", "tempfile"));

    assert(decompressFile("tests/extra1.huf", "tempfile"));
    assert(identicalFiles("tests/extra1.orig", "tempfile"));

    assert(decompressFile("tests/extra2.huf", "tempfile"));
    assert(identicalFiles("tests/extra2.orig", "tempfile"));

    assert(decompressFile("tests/extra3.huf", "tempfile"));
    assert(identicalFiles("tests/extra3.orig", "tempfile"));

    assert(decompressFile("tests/extra4.huf", "tempfile"));
    assert(identicalFiles("tests/extra4.orig", "tempfile"));

    assert(decompressFile("tests/extra5.huf", "tempfile"));
    assert(identicalFiles("tests/extra5.orig", "tempfile"));

    assert(decompressFile("tests/extra6.huf", "tempfile"));
    assert(identicalFiles("tests/extra6.orig", "tempfile"));

    assert(decompressFile("tests/extra7.huf", "tempfile"));
    assert(identicalFiles("tests/extra7.orig", "tempfile"));

    assert(decompressFile("tests/extra8.huf", "tempfile"));
    assert(identicalFiles("tests/extra8.orig", "tempfile"));

    assert(decompressFile("tests/extra9.huf", "tempfile"));
    assert(identicalFiles("tests/extra9.orig", "tempfile"));

    return 0;
}
#endif /* __PROGTEST__ */
