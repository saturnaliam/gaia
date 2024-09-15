#ifndef GAIA_HPP_
#define GAIA_HPP_

#include <string>
#include <vector>

namespace gaia {
    inline std::string compiler = "";
    inline std::string output_name = "";
    inline std::string input_directory = "";
    inline std::string output_directory = "";
    inline std::vector<std::string> flags;
    inline std::vector<std::string> input_files;

    auto build() -> void;
}

#endif // GAIA_HPP_