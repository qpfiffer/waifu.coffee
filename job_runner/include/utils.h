#include <msgpack.hpp>
#include <zmq.hpp>

namespace waifu {
    namespace utils {
        // Gets the size of a file in bytes. Probably.
        int get_file_size(const std::string &filepath);
        // Turns message_t objects into msgpack unpacked/object objects.
        void zmq_to_msgpack(zmq::message_t *request, msgpack::unpacked *unpacked_body, msgpack::object *obj);
    }
}
