#include <iostream>
#include "utils.h"

using namespace waifu;

msgpack::object *utils::zmq_to_msgpack(zmq::message_t *request) {
    msgpack::object *obj = new msgpack::object;
    // Convert to msgpack object
    msgpack::unpacked unpacked_body;
    msgpack::unpack(&unpacked_body, (char *)request->data(), request->size());

    *obj = unpacked_body.get();
    return obj;
}
