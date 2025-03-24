# about

gaia is a simple build tool for C++ projects.

# usage

there are 8 different variables to set when using gaia, but only 1 is required to be set.


| variable                 | usage                                                             | default                            |
| ------------------------ | ----------------------------------------------------------------- | -----------------------------------|
| `gaia_input_files`*     | the files to be compiled                                          | nothing                            |
| `gaia_echo`             | controls if the commands run should be printed to the console     | `false`                            |
| `gaia_force_compile`    | forces the code to recompile, even if nothing was changed         | `false`                            |
| `gaia_compiler`         | the compiler to be used                                           | whatever was used to compile`gaia` |
| `gaia_output_name`      | the name to be used for the executable                            | `main`                             |
| `gaia_input_directory`  | the directory input files are in, prepended directly to the files | nothing                            |
| `gaia_output_directory` | the directory to output the executable                            | nothing                            |
| `gaia_flags`            | the compilation flags to be used                                  | nothing                            |
| `gaia_extra_commands`   | any extra commands to be run                                      | nothing                            |

there are a number of helper functions to help with setting these values.
| function                                                       | usage                   |
| -------------------------------------------------------------- | ----------------------- |
| `add_file(const std::string &file)`                      | adds a single file.     |
| `add_files(const std::vector<std::string> &files)`       | adds multiple files.    |
| `add_flag(const std::string &flag)`                      | adds a single flag.     |
| `add_flags(const std::vector<std::string> &flags)`       | adds multiple flags.    |
| `add_command(const std::string &command)`                | adds a single command.  |
| `add_commands(const std::vector<std::string> &commands)` | adds multiple commands. |

during runtime, it sets a `compile_cmd` environment variable, which can be used in any extra commands to reference the command used.

# example
```cpp
/// inside gaia.cpp
auto main(void) -> int {
    gaia_compiler = "g++";
    gaia_output_directory = "build/";
    gaia_input_directory = "src/";

    add_files({ "file1.cpp", "file2.cpp" });
    add_flag("-Wall");
    add_flags({ "-Wextra", "-pedantic" });
    
    build();

    // this will run "g++ -o build/main -Wall -Wextra -pedantic src/file1.cpp src/file2.cpp"
}