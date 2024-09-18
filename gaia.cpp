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
    do {                                                  \
        fprintf(stderr, "[-] " msg "\n", ##__VA_ARGS__);  \
        std::exit(1);                                     \
    } while (0)

#define info(msg, ...) \
    printf("[i] " msg "\n", ##__VA_ARGS__)

#define echo(msg, ...) \
    if (gaia::echo) printf("" msg "\n", ##__VA_ARGS__)

auto add_many(const std::function<void(std::string)> &func,
    const std::vector<std::string> &input) -> void;
auto combine_vector(const std::vector<std::string> &input,
    const std::string &prefix = "") -> std::string;
auto get_file_mod_time(const std::string &filename) -> long;
auto fix_directory(std::string &directory) -> void;

auto recompile_gaia() -> void;
auto compilation_invalid() -> bool;
auto create_build_directory() -> void;

auto handle_flags(const std::vector<std::string> &args) -> void;

auto main(const int argc, const char **argv) -> int {
    handle_flags(std::vector<std::string>{argv, argv +argc});

    gaia::output_directory = "build";
    gaia::add_command("echo $compile_cmd");
    gaia::add_flags({ "-Wall", "-Wextra" });
    gaia::add_file("src/main.cpp");
    gaia::build();
    /* PLACE BUILD CODE HERE */
    return 0;
}

/**
 * @note this should be run last!
 * @brief builds the program.
 * 
 */
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

    if (gaia::input_files.size() == 0) error("no input files given!");

    fix_directory(gaia::output_directory);
    fix_directory(gaia::input_directory);

    const std::string files = combine_vector(gaia::input_files, gaia::input_directory);
    const std::string flags = combine_vector(gaia::flags);

    const std::string compile_command = std::format("{} -o {}{}{} {}",
        gaia::compiler, gaia::output_directory, gaia::output_name,
        (flags.length() > 0 ? " " + flags : ""),
        (files.length() > 0 ? " " + files : ""));

    if (setenv("compile_cmd", compile_command.c_str(), 1) == -1)
        error("could not set environment variable");

    if (compilation_invalid()) {
        info("compiling program");
        echo("%s", compile_command.c_str());

        if (std::system(compile_command.c_str()) != 0)
            error("error during compilation");
    } else {
        info("no code updated, skipping compilation");
    }

    for (const auto extra : gaia::extra_commands) {
        info("running extra command");
        echo("%s", extra.c_str());
        std::system(extra.c_str());
    }
}

/**
 * @brief recompiles gaia, if needed.
 * 
 */
auto recompile_gaia() -> void {
    const long gaia_mod_time = get_file_mod_time("gaia");
    const long gaia_src_mod_time = get_file_mod_time("gaia.cpp");

    // this probably means there's some fucked up file issue
    if (gaia_mod_time == -1 || gaia_src_mod_time == -1)
        error("error getting file modification times!");
    
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

        // we exit here, otherwise there's an infinite loop.
        std::exit(0);
    }
}

/**
 * @brief checks if the program has to be recompiled.
 * 
 * @return true the program must be recompiled
 * @return false the program won't be recompiled
 */
auto compilation_invalid() -> bool {
    if (gaia::force_compile) return true;

    const std::string output_file = std::format("{}{}",
        gaia::output_directory,
        gaia::output_name);
    
    const long output_mod_time = get_file_mod_time(output_file);

    // this (usually) means the output file doesn't exist yet
    if (output_mod_time == -1) return true;

    for (std::string file : gaia::input_files) {
        file = std::format("{}{}", gaia::input_directory, file);
        const long file_mod_time = get_file_mod_time(file);

        if (file_mod_time == -1) return true;

        if (file_mod_time > output_mod_time) return true;
    }

    return false;
}

/**
 * @brief handles the flags passed into the program.
 * 
 * @param args argv
 */
auto handle_flags(const std::vector<std::string> &args) -> void {
    for (auto arg = args.begin() + 1; arg != args.end(); arg++) {
        const auto flag = [&arg](const std::set<std::string> &flags) -> bool {
            return flags.contains(*arg);
        };

        if (flag({ "-f", "--force" })) {
            gaia::force_compile = true;
        } else if (flag({ "-e", "--echo" })) {
            gaia::echo = true;
        }
    }
}

/**
 * @brief if the build directory doesn't exist, create it.
 * 
 */
auto create_build_directory() -> void {
    namespace fs = std::filesystem;
    if (!fs::exists(gaia::output_directory)) {
        fs::create_directory(gaia::output_directory);
    }
}

/**
 * @brief combines a vecctor into a space separated string.
 * 
 * @param input the input vector
 * @param prefix (optional) a prefix to be appended to each element
 * @return std::string the new, space separated string
 */
auto combine_vector(const std::vector<std::string> &input, const std::string &prefix) -> std::string {
    if (input.size() == 0) return "";

    std::string combined = std::accumulate(input.begin(), input.end(), std::string(""), 
        [prefix](auto a, auto b) {
            return prefix + std::move(a) + " " + std::move(b);
        });

    return combined;
}

/**
 * @brief runs a function on every element of a vector
 * 
 * @param func the function to be run
 * @param input the input vector
 */
auto add_many(const std::function<void(std::string)> &func, const std::vector<std::string> &input) -> void {
    std::for_each(input.begin(), input.end(),
        [&func](const auto &e) { func(e); } );
}

/**
 * @brief returns the modification date of the file
 * 
 * @param filename the file
 * @return long the modificiation date OR -1 on failure
 */
auto get_file_mod_time(const std::string &filename) -> long {
    struct stat file_stat;

    if (stat(filename.c_str(), &file_stat) != 0) return -1;

    return file_stat.st_mtim.tv_sec;
}

/**
 * @brief inserts a / at the end of a string if it doesn't exist
 * 
 * @param directory the directory to modify.
 */
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