#include "gaia.hpp"

#include <cstdio>
#include <filesystem>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstdlib>

auto main(void) -> int {
    gaia::build();
    return 0;
}

#define error(msg, ...)                                   \
    fprintf(stderr, "[-] " msg "\n", ##__VA_ARGS__); \
    std::exit(1)

#define info(msg, ...) \
    printf("[i] " msg "\n", ##__VA_ARGS__)

auto recompile_gaia() -> void;

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
        
        // checks if 
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