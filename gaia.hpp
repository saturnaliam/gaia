// gaia is the most GOATED build tool for C++, created by Lucia Cover

#ifndef GAIA_HPP_
#define GAIA_HPP_

#include <string>
#include <vector>

namespace gaia {
    inline bool echo = false;                       // if the commands that are run should be echoed
    inline bool force_compile = false;              // if compilation should be forced, even if no code was updated
    inline std::string compiler = "";               // the compiler to use, defaults to the one used to compile gaia
    inline std::string output_name = "main";        // the output filename, defaults to main
    inline std::string input_directory = "";        // the directory that contains the input files, appended to the start of each input file
    inline std::string output_directory = "";       // the directory to output into
    inline std::vector<std::string> flags;          // compile flags to use
    inline std::vector<std::string> input_files;    // the files to compile
    inline std::vector<std::string> extra_commands; // any extra commands to run

    auto build() -> void;

    auto add_file(const std::string &file) -> void;
    auto add_flag(const std::string &flag) -> void;
    auto add_command(const std::string &command) -> void;

    auto add_files(const std::vector<std::string> &files) -> void;
    auto add_flags(const std::vector<std::string> &flags) -> void;
    auto add_commands(const std::vector<std::string> &command) -> void;
}

#endif // GAIA_HPP_