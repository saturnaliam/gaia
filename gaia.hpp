#ifndef GAIA_HPP_
#define GAIA_HPP_

#include <string>
#include <vector>

namespace gaia {
    inline std::string compiler = "";
    inline std::string output_name = "";
    inline std::vector<std::string> flags;

    auto build() -> void;
}

#endif // GAIA_HPP_