#include <iostream>
#include "waifu.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "You need to specify a URI.\n";
        return -1;
    }
    return waifu::main_loop(argc, argv);
}
