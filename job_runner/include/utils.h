#include <msgpack.hpp>
#include <zmq.hpp>

namespace waifu {
    namespace utils {
        msgpack::object *zmq_to_msgpack(zmq::message_t *request);
    }
}
