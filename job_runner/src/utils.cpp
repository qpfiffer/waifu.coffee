#include "utils.h"

using namespace waifu;

void utils::zmq_to_msgpack(zmq::message_t *request, msgpack::object *obj) {
    // Convert to string
    std::string raw_data = static_cast<char*>(request->data());

    // Convert to msgpack object
    msgpack::unpacked unpacked_body;
    msgpack::unpack(&unpacked_body, raw_data.data(), raw_data.size());
    *obj = unpacked_body.get();
}
