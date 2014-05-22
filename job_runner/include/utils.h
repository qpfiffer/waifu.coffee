#include <msgpack.hpp>
#include <zmq.hpp>

namespace waifu {
    namespace utils {
        void zmq_to_msgpack(zmq::message_t *request, msgpack::object *obj);
    }
}
