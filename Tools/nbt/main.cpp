#include <iostream>
#include <filesystem>

#include "nbt/nbt.hpp"

namespace fs = std::experimental::filesystem;

enum class operation : int {
    list = 0,
    merge,
    help
};
struct script_options {
    std::string path = ".";
    std::string output = "output";
    std::vector<std::string> nbt_inputs;
    operation op = operation::list;
};
script_options parseArguments(int argc, char** argv);
void list(script_options& opts);
void merge(script_options& opts);

int main(int argc, char** argv) {
    auto opts = parseArguments(argc, argv);

    // print out what the script is going to do
    /*
    switch (opts.op) {
    case operation::help:
        std::cout << "Help" << std::endl;
        break;
    case operation::list:
        std::cout << "List" << std::endl;
        break;
    case operation::merge:
        std::cout << "Merge {";
        for (int n = 0; n < opts.nbt_inputs.size(); n++) {
            std::cout << opts.nbt_inputs[n];
            if (n < opts.nbt_inputs.size()-1)
                std::cout << ",";
        }
        std::cout << "} into: \"" << opts.output << "\"" << std::endl;
        break;
    }
    */

    // perform the operations
    switch (opts.op) {
    case operation::help:
        std::cout << "usage:          nbt [operation] {inputs} {optional flags}" << std::endl;
        std::cout << std::endl;
        std::cout << "operations:     -h              Show this help message." << std::endl;
        std::cout << "                -merge          Merge .nbt files together." << std::endl;
        std::cout << "                -l              List all .nbt files in specified directory." << std::endl;
        std::cout << std::endl;                       
        std::cout << "optional flags: -o <filename>   Set <filename> as the output file" << std::endl;
        std::cout << "                -p <path>       Set <path> as the search path" << std::endl;
        std::cout << std::endl;
        break;
    case operation::list:
        list(opts);
        break;
    case operation::merge:
        merge(opts);
        break;
    }

    return 0;
}

script_options parseArguments(int argc, char** argv) {
    std::vector<std::string> args;

    // seed with data if run from visual studio
    std::string runcmd(argv[0]);
    if (runcmd.find(".exe") != std::string::npos) {
        std::cout << "Run from visual studio" << std::endl;
        args.push_back("nbt.exe");
        args.push_back("-merge");
        args.push_back("output.nbt_file");
        args.push_back("bigtest.nbt");
        args.push_back("-o");
        args.push_back("merged.nbt");
        args.push_back("-p");
        args.push_back("x64/Debug/files");

        //merge bigtest.nbt outout.nbt_file - o merged.nbt - p files

        argc = args.size();
    }
    else {
        for (int n = 0; n < argc; n++) {
            args.emplace_back(argv[n]);
        }
    }


    script_options opts;
    for (int n = 1; n < argc; n++) {
        if (args[n].compare("-h") == 0) {
            opts.op = operation::help;
            return opts;
        }
        if (args[n].compare("-merge") == 0) {
            opts.op = operation::merge;

            int start = n;
            for (int i = start + 1; i < argc; i++) {
                if (args[i][0] == '-')
                    break;

                opts.nbt_inputs.push_back(args[i]);
                n++;
            }
            continue;
        }
        if (args[n].compare("-l") == 0) {
            opts.op = operation::list;
            continue;
        }
        if (args[n].compare("-o") == 0) {
            if (n < argc && args[n + 1][0] == '-') // no output filename found
                break;

            opts.output = args[n + 1];
            n++;
            continue;
        }
        if (args[n].compare("-p") == 0) {
            if (n < argc && args[n + 1][0] == '-') // no path found
                break;

            opts.path = args[n + 1];
            n++;
            continue;
        }

        // not a recognized command
        if (args[n].find_first_of('-') == 0) {
            std::cout << "Unrecognized command: " << args[n] << std::endl << std::endl;
            opts.op = operation::help;
            return opts;
        }
    }

    return opts;
}

void merge(script_options& opts) {
    if (opts.op != operation::merge)
        return;

    std::cout << "Merging " << opts.nbt_inputs.size() << " file(s)!" << std::endl;
    std::cout << "Output: " << opts.output << std::endl;
    std::cout << "Path: " << opts.path << std::endl;
    if (opts.nbt_inputs.size() == 0) return;
    if (opts.path.size() == 0) return;

    // append filesep if needed
    std::vector<std::string> fullFiles;
    int numFiles = opts.nbt_inputs.size();
    fullFiles.reserve(numFiles);
    for (int n = 0; n < numFiles; n++) {
            fullFiles.push_back((fs::absolute(opts.path) / opts.nbt_inputs[n]).string());
    }

    std::string compound_name = "compound";
    nbt::nbt_byte version_major = 0; // need to choose this based on the input files?
    nbt::nbt_byte version_minor = 1;
    endian::endian endianness = endian::little;
    nbt::tag_compound compound;

    for (auto p : fullFiles) {
        std::ifstream file(p, std::ios::binary);
        if (file) {

            nbt::nbt_byte header[32];
            memset(header, 0, 32);
            file.read(reinterpret_cast<char*>(header), 32);

            char sig[5];
            sig[4] = 0;
            memcpy(sig, header, 4);
            if (strcmp(sig, "1234") != 0) {
                std::cout << fs::path(p).filename() << " is not a valid .nbt file!" << std::endl;
                return;
            }
            else {
                nbt::nbt_byte version_major = header[4];
                nbt::nbt_byte version_minor = header[5];
                endian::endian endianness   = header[6] == 1 ? endian::big : endian::little;

                std::cout << fs::path(p) << " loaded. NBT Version " << (int)version_major << "." << (int)version_minor << ", " << (endianness == endian::big ? "big" : "little") << std::endl;

                auto rd = nbt::read_compound_raw(file, endianness);
                compound.insert(rd.first, nbt::tag_compound(rd.second->as<nbt::tag_compound>()));
            }

            file.close();
        }
    }

    nbt::file_data ddd = { std::move(compound_name), std::move(std::make_unique<nbt::tag_compound>(compound)) };
    nbt::write_to_file(opts.output, ddd, version_major, version_minor, endianness);
}

void list(script_options& opts) {
    std::string fullpath = fs::absolute(opts.path).string();
    std::cout << "Listing all files on path: <" << fullpath << ">:\n" << std::endl;
    int numFound = 0;
    for (const auto& entry : fs::directory_iterator(fullpath)) {
        if (entry.path().extension().string().compare(".exe") == 0 || 
            fs::is_directory(entry.path())// ||
            //entry.path().extension().string().compare(".nbt") != 0 ||
            //entry.path().extension().string().compare(".nbt_file") != 0
            ) {
            // skip
            continue;
        }

        std::ifstream file(entry.path(), std::ios::binary);
        if (file) {
            // file opened properly
            nbt::nbt_byte header[32];
            memset(header, 0, 32);

            file.read(reinterpret_cast<char*>(header), 32);
            if (file) {
                // 32 bytes were read properly
                char sig[5];
                sig[4] = 0;
                memcpy(sig, header, 4);
                if (strcmp(sig, "1234") == 0) {
                    // has the correct signature
                    nbt::nbt_byte version_major = header[4];
                    nbt::nbt_byte version_minor = header[5];
                    endian::endian endianness = header[6]==1 ? endian::big : endian::little;

                    std::cout 
                        << "  Version " << (int)version_major << "." << (int)version_minor << "\t " 
                        << (endianness == endian::big ? "B" : "L") << "E\t"
                        << entry.path().filename()
                        << std::endl;
                    numFound++;
                }
            }

            file.close();
        }
    }
    std::cout << std::endl << numFound << " files found." << std::endl;
}