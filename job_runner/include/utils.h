#include <msgpack.hpp>
#include <zmq.hpp>

namespace waifu {
    namespace utils {
        // Gets the size of a file in bytes. Probably.
        int get_file_size(const std::string &filepath);
        // Scales an image to what Haar2D expects. Fills out dest_array.
        void scale_image(const unsigned char *source_array, const int &s_width, const int &s_height,
                         unsigned char *dest_array);
        // Turns message_t objects into msgpack unpacked/object objects.
        void zmq_to_msgpack(zmq::message_t *request, msgpack::unpacked *unpacked_body, msgpack::object *obj);
    }
}
