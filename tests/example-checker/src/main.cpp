//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <filesystem>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cstdlib>

std::string read_text_file(const std::filesystem::path& path)
{
    if(!exists(path))
    {
        return std::string{};
    }

    auto oss = std::ostringstream{};
    oss << std::ifstream{path}.rdbuf();
    return oss.str();
}

int main(int /*argc*/, char** argv)
{
    const auto bin_path = argv[1];
    const auto expected_output_file_path = argv[2];

    const auto cmd = std::string{bin_path} + " > output.txt";
    const auto cmd_exit_code = std::system(cmd.c_str());
    const auto output = read_text_file("output.txt");

    if (cmd_exit_code != 0)
    {
        std::cout << "Execution failed with code " << cmd_exit_code << '\n';
        std::cout << "Output:\n" << output << '\n';
        return 1;
    }

    const auto expected_output = read_text_file(expected_output_file_path);

    if (output != expected_output)
    {
        std::cout << "Expected output:\n" << expected_output << '\n';
        std::cout << "Actual output:\n" << output << '\n';
        return 1;
    }
    return 0;
}
