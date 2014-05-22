#include <iostream>
#include "utils.h"

using namespace waifu;

void utils::zmq_to_msgpack(zmq::message_t *request, msgpack::unpacked *unpacked_body, msgpack::object *obj) {
    msgpack::unpack(unpacked_body, (char *)request->data(), request->size());
    *obj = unpacked_body->get();
}
