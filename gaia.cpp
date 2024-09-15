// gaia is the most GOATED build tool for C++, created by Lucia Cover

#include "gaia.hpp"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <numeric>
#include <sys/stat.h>
#include <sys/types.h>

#define error(msg, ...)                                   \
    fprintf(stderr, "[-] " msg "\n", ##__VA_ARGS__); \
    std::exit(1)

#define info(msg, ...) \
    printf("[i] " msg "\n", ##__VA_ARGS__)

#define echo(msg, ...) \
    if (gaia::echo) printf("" msg "\n", ##__VA_ARGS__)

auto main(const int argc, const char **argv) -> int {
    if (argc > 1) {
        // only one argument is supported rn, so we only care abt one
        const auto arg = std::string(argv[1]);
        if (arg == "-f" || arg == "--force") {
            gaia::force_compile = true;
        } else if (arg == "-e" || arg == "--echo") {
            gaia::echo = true;
        }
    }

    gaia::output_directory = "build/";
    gaia::add_file("src/main.cpp");
    gaia::add_command("echo $compile_cmd");
    gaia::build();
    /* PLACE BUILD CODE HERE */
    return 0;
}

auto recompile_gaia() -> void;
auto compilation_invalid() -> bool;
auto create_build_directory() -> void;

auto combine_vector(const std::vector<std::string> &input, const std::string &prefix = "") -> std::string;

auto gaia::build() -> void {
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

    const std::string files = combine_vector(gaia::input_files, gaia::input_directory);
    const std::string flags = combine_vector(gaia::flags);

    const std::string command = gaia::compiler +
        " -o " + gaia::output_directory + gaia::output_name +
        " " + flags
        + " " + files;

    if (setenv("compile_cmd", command.c_str(), 1) == -1) {
        error("could not set environment variable");
    }

    if (compilation_invalid()) {
        info("compiling program");
        echo("%s", command.c_str());
        std::system(command.c_str());
    } else {
        info("no code updated, skipping compilation");
    }

    for (const auto extra : gaia::extra_commands) {
        info("running extra command");
        echo("%s", command.c_str());
        std::system(extra.c_str());
    }
}

auto gaia::add_command(const std::string &command) -> void {
    gaia::extra_commands.push_back(command);
}

auto gaia::add_commands(const std::vector<std::string> &commands) -> void {
    std::for_each(commands.begin(), commands.end(),
        [](const auto &command) { gaia::add_command(command); });
}

auto gaia::add_file(const std::string &file) -> void {
    gaia::input_files.push_back(file);
}

auto gaia::add_files(const std::vector<std::string> &files) -> void {
    std::for_each(files.begin(), files.end(),
        [](const auto &file) { gaia::add_file(file); });
}

auto gaia::add_flag(const std::string &flag) -> void {
    gaia::flags.push_back(flag);
}

auto gaia::add_flags(const std::vector<std::string> &flags) -> void {
    std::for_each(flags.begin(), flags.end(),
        [](const auto &flag) { gaia::add_flag(flag); });
}

// checks if the program needs to be recompiled
auto recompile_gaia() -> void {
    const char *gaia_src_file = "gaia.cpp";
    const char *gaia_file = "gaia";
    struct stat gaia_src_stat;
    struct stat gaia_stat;

    if (stat(gaia_src_file, &gaia_src_stat) == 0 && stat(gaia_file, &gaia_stat) == 0) {
        const auto src_time = gaia_src_stat.st_mtim;
        const auto gaia_time = gaia_stat.st_mtim;
        
        // checks if the source code has changed since gaia was last compiled
        if (src_time.tv_sec > gaia_time.tv_sec) {
            info("recompiling gaia");
            const std::string compile_command = gaia::compiler + " -o gaia gaia.cpp";
            const std::string run_command = std::string("./gaia") + (gaia::force_compile ? " -f" : "");
            std::system(compile_command.c_str());
            std::system(run_command.c_str());
            std::exit(0);
        }
    } else {
        error("could not determine file statistics.");
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