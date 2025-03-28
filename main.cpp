#include "first_app.hpp"

// std libs
#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main() {
    sve::FirstApp app{};

    try {
        app.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}