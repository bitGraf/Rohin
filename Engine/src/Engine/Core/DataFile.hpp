#pragma once

#include "Engine/Core/Base.hpp"

#include <vector>
#include <string>
#include <fstream>

struct DataFile {
    struct Block {
        u8 NameSize;
        std::string Name;
        u32 Size;
        u32 Offset;

        std::vector<u8> data;

        Block() :Size(0), Offset(0), NameSize(0), loc(0) {}
        Block(const std::string& name)
            : Name(name), Offset(0), Size(0), loc(0) {
            NameSize = name.size();

            push_string(name);
        }

        void push_byte(u8 value) {
            data.push_back(value);
            Size += sizeof(value);
        }
        void push_short(u16 value) {
            u8* data = reinterpret_cast<u8*>(&value);
            push_byte(data[0]);
            push_byte(data[1]);
        }
        void push_string(const std::string& str) {
            u8 len = str.size();
            push_byte(len);
            for (u8 n = 0; n < len; n++) {
                push_byte(str[n]);
            }

        }
        void push_data(u8* value, size_t size) {
            for (int n = 0; n < size / sizeof(u8); n++) {
                push_byte(value[n]);
            }
        }

        template<typename T>
        void push(T value) {
            assert(typeid(T) != typeid(std::string));
            push_data(reinterpret_cast<u8*>(&value), sizeof(value));
        }

        u8 read_byte() {
            u8 out = data[loc];
            loc++;
            return out;
        }
        u16 read_short() {
            u16 out;
            memcpy(&out, &data[loc], 2);
            loc += 2;
            return out;
        }
        std::string read_string(u8* nameSize = nullptr) {
            u8 outSize = read_byte();
            std::string out(reinterpret_cast<char*>(&data[loc]), outSize);
            loc += outSize;
            if (nameSize)
                *nameSize = outSize;
            return out;
        }
        void read_data(u8* dst, size_t dataSize) {
            assert(dst);
            memcpy(dst, &data[loc], dataSize);
            loc += dataSize;
        }

        template<typename T>
        T read() {
            assert(typeid(T) != typeid(std::string));
            T out;
            read_data(reinterpret_cast<u8*>(&out), sizeof(out));
            return out;
        }

    private:
        size_t loc = 0;
    };

    DataFile() {}

    void AddBlock(Block& block) {
        m_Blocks.insert(std::make_pair(block.Name, block));
    }

    void ResolveOffsets() {
        u16 blockStart = std::string("BlockTable").size();
        blockStart += sizeof(u8); // numBlocks

        for (auto blockKV : m_Blocks) {
            auto& block = blockKV.second;
            blockStart += block.NameSize;
            blockStart += sizeof(block.NameSize);
            blockStart += sizeof(block.Offset);
            blockStart += sizeof(block.Size);
        }
        fileLength = blockStart; // to check later

        for (auto blockKV : m_Blocks) {
            auto& block = blockKV.second;
            block.Offset = blockStart;
            m_Blocks[block.Name] = block;
            blockStart += block.Size;
            fileLength = blockStart;
        }

        bool done = true;
    }

    void WriteToFile(const std::string& filename) {
        // open file for writing
        std::ofstream fp(filename, std::ios::out | std::ios::binary);

        // write file header
        fp << "BlockTable";

        u8 numBlocks = m_Blocks.size();
        fp.write(reinterpret_cast<char*>(&numBlocks), sizeof(numBlocks));

        for (auto blockKV : m_Blocks) {
            auto& block = blockKV.second;

            fp.write(reinterpret_cast<char*>(&block.NameSize), sizeof(block.NameSize));
            fp.write(reinterpret_cast<char*>(&block.Name[0]), block.NameSize);
            fp.write(reinterpret_cast<char*>(&block.Offset), sizeof(block.Offset));
            fp.write(reinterpret_cast<char*>(&block.Size), sizeof(block.Size));
        }

        for (auto blockKV : m_Blocks) {
            auto& block = blockKV.second;

            auto pos = fp.tellp();
            ENGINE_LOG_ASSERT(pos.seekpos() == block.Offset, "");

            fp.write(reinterpret_cast<char*>(&block.data[0]), block.data.size() * sizeof(block.data[0]));
        }

        auto end = fp.tellp().seekpos();
        fp.close();

        ENGINE_LOG_ASSERT(end == fileLength, "");
    }

    void ReadFromFile(const std::string& filename) {
        // open file for reading
        std::ifstream fp(filename, std::ios::in | std::ios::binary);

        // read file header
        //fp << "BlockTable";
        fp.seekg(10, std::ios::beg);

        u8 numBlocks = 0;
        fp.read(reinterpret_cast<char*>(&numBlocks), sizeof(numBlocks));
        for (int n = 0; n < numBlocks; n++) {
            Block block;
            fp.read(reinterpret_cast<char*>(&block.NameSize), sizeof(block.NameSize));
            char* name = new char[block.NameSize];
            fp.read(name, block.NameSize);
            block.Name = std::string(name, block.NameSize);
            delete[] name;
            fp.read(reinterpret_cast<char*>(&block.Offset), sizeof(block.Offset));
            fp.read(reinterpret_cast<char*>(&block.Size), sizeof(block.Size));
            m_Blocks[block.Name] = block;
        }

        for (auto blockKV : m_Blocks) {
            auto& block = blockKV.second;

            u8* data = new u8[block.Size];
            fp.read(reinterpret_cast<char*>(data), block.Size);
            block.data.reserve(block.Size);
            block.data.assign(data, data + block.Size);
            delete[] data;

            m_Blocks[block.Name] = block;
        }

        ENGINE_LOG_ASSERT(!fp.eof(), "");
        fileLength = fp.tellg().seekpos();

        fp.close();

    }

    Block& GetBlock(const std::string& blockName) {
        auto& b = m_Blocks[blockName];
        b.read_string();// eat through the block name
        return b;
    }

    std::unordered_map<std::string, Block> m_Blocks;
    size_t fileLength;
};