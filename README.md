# about

gaia is a simple build tool for C++ projects.

# usage

there are 8 different variables to set when using gaia, but only 1 is required to be set.


| variable                 | usage                                                             | default                            |
| ------------------------ | ----------------------------------------------------------------- | -----------------------------------|
| `gaia::input_files`*     | the files to be compiled                                          | nothing                            |
| `gaia::echo`             | controls if the commands run should be printed to the console     | `false`                            |
| `gaia::force_compile`    | forces the code to recompile, even if nothing was changed         | `false`                            |
| `gaia::compiler`         | the compiler to be used                                           | whatever was used to compile`gaia` |
| `gaia::output_name`      | the name to be used for the executable                            | `main`                             |
| `gaia::input_directory`  | the directory input files are in, prepended directly to the files | nothing                            |
| `gaia::output_directory` | the directory to output the executable                            | nothing                            |
| `gaia::flags`            | the compilation flags to be used                                  | nothing                            |
| `gaia::extra_commands`   | any extra commands to be run                                      | nothing                            |

there are a number of helper functions to help with setting these values.
| function                                                       | usage                   |
| -------------------------------------------------------------- | ----------------------- |
| `gaia::add_file(const std::string &file)`                      | adds a single file.     |
| `gaia::add_files(const std::vector<std::string> &files)`       | adds multiple files.    |
| `gaia::add_flag(const std::string &flag)`                      | adds a single flag.     |
| `gaia::add_flags(const std::vector<std::string> &flags)`       | adds multiple flags.    |
| `gaia::add_command(const std::string &command)`                | adds a single command.  |
| `gaia::add_commands(const std::vector<std::string> &commands)` | adds multiple commands. |

# example
```cpp
/// inside gaia.cpp
auto main(void) -> int {
    gaia::compiler = "g++";
    gaia::output_directory = "build/";
    gaia::input_directory = "src/";

    gaia::add_files({ "file1.cpp", "file2.cpp" });
    gaia::add_flag("-Wall");
    gaia::add_flags({ "-Wextra", "-pedantic" });
    
    gaia::build();

    // this will run "g++ -o build/main -Wall -Wextra -pedantic src/file1.cpp src/file2.cpp"
}