#include "gaia.hpp"

#include <cstdio>
#include <numeric>
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <cstdlib>

auto main(void) -> int {
    gaia::input_files = { "main.cpp", "afds.cpp" };
    gaia::build();
    return 0;
}

#define error(msg, ...)                                   \
    fprintf(stderr, "[-] " msg "\n", ##__VA_ARGS__); \
    std::exit(1)

#define info(msg, ...) \
    printf("[i] " msg "\n", ##__VA_ARGS__)


auto recompile_gaia() -> void;

auto combine_vector(const std::vector<std::string> &input) -> std::string;

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

    if (gaia::input_files.size() == 0) {
        error("no input files given!");
    }

    const std::string files = combine_vector(gaia::input_files);
    std::cout << files;
}

// checks if the program needs to be recompiled
auto recompile_gaia() -> void {
    const char *gaia_src_file = "gaia.cpp";
    const char *gaia_file = "gaia";
    struct stat gaia_src_result;
    struct stat gaia_result;

    if (stat(gaia_src_file, &gaia_src_result) == 0 && stat(gaia_file, &gaia_result) == 0) {
        auto src_time = gaia_src_result.st_mtim;
        auto gaia_time = gaia_result.st_mtim;
        
        // checks if the source code has changed since gaia was last compiled
        if (src_time.tv_sec > gaia_time.tv_sec) {
            info("recompiling gaia");
            const std::string compile_command = gaia::compiler + " -o gaia gaia.cpp";
            std::system(compile_command.c_str());
            std::system("./gaia");
            std::exit(0);
        }
    } else {
        error("could not determine file statistics.");
    }
}

// combines values of a string array into a new space separated string
auto combine_vector(const std::vector<std::string> &input) -> std::string {
    std::vector<std::string> input_spaces(input.size());
    
    std::transform(input.begin(), input.end(), input_spaces.begin(),
        [](const auto &element) { return element + " "; });


    return std::reduce(input_spaces.begin(), input_spaces.end(), std::string(""));
}