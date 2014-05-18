#pragma once
#include <msgpack.hpp>

using namespace msgpack;

namespace waifu {
    int main_loop(int argc, char *argv[]);

    class waifuProcessor {
        public:
            sbuffer *process_request(unpacked *request);
    };
}
