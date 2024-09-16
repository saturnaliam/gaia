// gaia is the most GOATED build tool for C++, created by Lucia Cover

#include "gaia.hpp"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <format>
#include <functional>
#include <numeric>
#include <set>
#include <sys/stat.h>
#include <sys/types.h>

#define error(msg, ...)                                   \
    fprintf(stderr, "[-] " msg "\n", ##__VA_ARGS__); \
    std::exit(1)

#define info(msg, ...) \
    printf("[i] " msg "\n", ##__VA_ARGS__)

#define echo(msg, ...) \
    if (gaia::echo) printf("" msg "\n", ##__VA_ARGS__)

auto add_many(const std::function<void(std::string)> &func, const std::vector<std::string> &input) -> void;
auto combine_vector(const std::vector<std::string> &input, const std::string &prefix = "") -> std::string;
auto get_file_mod_time(const std::string &filename) -> long;
auto fix_directory(std::string &directory) -> void;

auto recompile_gaia() -> void;
auto compilation_invalid() -> bool;
auto create_build_directory() -> void;

auto handle_flags(const std::vector<std::string> &args) -> void;

auto main(const int argc, const char **argv) -> int {
    handle_flags(std::vector<std::string>{argv, argv +argc});

    gaia::output_directory = "build";
    gaia::add_file("src/main.cpp");
    gaia::build();
    /* PLACE BUILD CODE HERE */
    return 0;
}

auto gaia::build() -> void {
    // setting the compiler to the one used for gaia, if none is given
    if (gaia::compiler == "") {
        #if defined(__clang__)
        gaia::compiler = "clang++";
        #elif defined(__GNUG__)
        gaia::compiler = "g++";
        #else
        #warning could not detect valid compiler!
        error("no valid compiler given!");
        #endif
    }

    recompile_gaia();
    create_build_directory();

    if (gaia::input_files.size() == 0) {
        error("no input files given!");
    }

    fix_directory(gaia::output_directory);
    fix_directory(gaia::input_directory);

    const std::string files = combine_vector(gaia::input_files, gaia::input_directory);
    const std::string flags = combine_vector(gaia::flags);

    const std::string compile_command = std::format("{} -o {}{}{} {}",
        gaia::compiler, gaia::output_directory, gaia::output_name,
        (flags.length() > 0 ? " " + flags : ""),
        (files.length() > 0 ? " " + files : ""));

    if (setenv("compile_cmd", compile_command.c_str(), 1) == -1) {
        error("could not set environment variable");
    }

    if (compilation_invalid()) {
        info("compiling program");
        echo("%s", compile_command.c_str());
        if (std::system(compile_command.c_str()) != 0) error("error during compilation");
    } else {
        info("no code updated, skipping compilation");
    }

    for (const auto extra : gaia::extra_commands) {
        info("running extra command");
        echo("%s", extra.c_str());
        std::system(extra.c_str());
    }
}

// checks if the program needs to be recompiled
auto recompile_gaia() -> void {
    const long gaia_mod_time = get_file_mod_time("gaia");
    const long gaia_src_mod_time = get_file_mod_time("gaia.cpp");

    if (gaia_mod_time == -1 || gaia_src_mod_time == -1) { error("error getting file modification times!"); }
    
    // checks if the source code has changed since gaia was last compiled
    if (gaia_src_mod_time > gaia_mod_time) {
        info("recompiling gaia");
        const std::string compile_command = std::format("{} -std=c++20 -o gaia gaia.cpp",
            gaia::compiler); 

        const std::string run_command = std::format("./gaia{}{}",
            (gaia::force_compile ? " -f" : ""),
            (gaia::echo ? " -e" : ""));
        
        std::system(compile_command.c_str());
        std::system(run_command.c_str());
        std::exit(0);
    }
}

// returns if the files have to be recompiled
auto compilation_invalid() -> bool {
    if (gaia::force_compile) return true;
    const std::string output_file = gaia::output_directory + gaia::output_name;
    struct stat output_stat;

    if (stat(output_file.c_str(), &output_stat) != 0) return true;

    for (std::string file : gaia::input_files) {
        file = gaia::input_directory + file;
        struct stat file_stat;

        if (stat(file.c_str(), &file_stat) != 0) return true;

        const auto file_mod_time = file_stat.st_mtim.tv_sec;
        const auto output_mod_time = output_stat.st_mtim.tv_sec;

        if (file_mod_time > output_mod_time) return true;
    }

    return false;
}

auto handle_flags(const std::vector<std::string> &args) -> void {

    for (auto arg = args.begin() + 1; arg != args.end(); arg++) {
        const auto flag = [&arg](std::set<std::string> flags) -> bool {
            const auto search = flags.find(*arg);
            return search != flags.end();
        };

        if (flag({ "-f", "--force" })) {
            gaia::force_compile = true;
        } else if (flag({ "-e", "--echo" })) {
            gaia::echo = true;
        }
    }
}

// creates build directory if it doesn't exist
auto create_build_directory() -> void {
    namespace fs = std::filesystem;
    if (!fs::exists(gaia::output_directory)) {
        fs::create_directory(gaia::output_directory);
    }
}

// combines a vector into a space separated string
auto combine_vector(const std::vector<std::string> &input, const std::string &prefix) -> std::string {
    if (input.size() == 0) return "";

    std::vector<std::string> input_spaces(input.size());
    
    std::transform(input.begin(), input.end(), input_spaces.begin(),
        [prefix](const auto &element) { return prefix + element + " "; });

    std::string combined = std::reduce(input_spaces.begin(), input_spaces.end(), std::string(""));
    combined.pop_back(); // hacky way to get rid of the final space
    return combined;
}

// runs foreach on each member of a vector
auto add_many(const std::function<void(std::string)> &func, const std::vector<std::string> &input) -> void {
    std::for_each(input.begin(), input.end(),
        [&func](const auto &e) { func(e); } );
}

// gets the time the file was last modified, -1 on failure.
auto get_file_mod_time(const std::string &filename) -> long {
    struct stat file_stat;

    if (stat(filename.c_str(), &file_stat) != 0) return -1;

    return file_stat.st_mtim.tv_sec;
}

// if a directory doesn't end in /, insert it.
auto fix_directory(std::string &directory) -> void {
    if (!directory.ends_with('/') && directory.length() > 0) {
        directory.push_back('/');
    }
}

auto gaia::add_command(const std::string &command) -> void {
    gaia::extra_commands.push_back(command);
}

auto gaia::add_commands(const std::vector<std::string> &commands) -> void {
    add_many(gaia::add_command, commands);
}

auto gaia::add_file(const std::string &file) -> void {
    gaia::input_files.push_back(file);
}

auto gaia::add_files(const std::vector<std::string> &files) -> void {
    add_many(gaia::add_file, files);
}

auto gaia::add_flag(const std::string &flag) -> void {
    gaia::flags.push_back(flag);
}

auto gaia::add_flags(const std::vector<std::string> &flags) -> void {
    add_many(gaia::add_flag, flags);
}