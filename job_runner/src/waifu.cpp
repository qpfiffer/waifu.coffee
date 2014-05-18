#include <iostream>
#include <kcpolydb.h>
#include <msgpack.hpp>
#include <zmq.hpp>
#include "waifu.h"

using namespace std;
using namespace kyotocabinet;

msgpack::sbuffer *waifu::waifuProcessor::process_request(msgpack::unpacked *request) {
    // Setup our response object
    std::map<std::string, bool> response_list;
    response_list["success"] = false;

    // Convert the request into something useful
    msgpack::object obj = request->get();

    std::map<std::string, std::string> conv_request;
    obj.convert(&conv_request);

    if (conv_request["cmd"] == "query") {
        std::string filepath = conv_request["filepath"];
        std::cout << "Filepath: " << filepath << std::endl;
        response_list["success"] = true;
    } else {
        response_list["success"] = false;
    }

    msgpack::sbuffer *to_return = new msgpack::sbuffer;
    msgpack::pack(to_return, response_list);

    return to_return;
}

int waifu::main_loop(int argc, char *argv[]) {
    char *URI = argv[1];

    waifuProcessor mainProcessor;

    // Prepare our context and socket
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REP);
    socket.bind(URI);

    while (true) {
        // Wait for next request from client
        zmq::message_t request;
        socket.recv(&request);

        // Convert to string
        std::string raw_data = static_cast<char*>(request.data());
        // Convert to msgpack object
        msgpack::unpacked unpacked_body;
        msgpack::unpack(&unpacked_body, raw_data.data(), raw_data.size());

        // Process the job
        msgpack::sbuffer *result = mainProcessor.process_request(&unpacked_body);

        // Copy result data into response buffer
        zmq::message_t response(result->size());
        memcpy((void *)response.data(), result->data(), result->size());

        // Send it back
        socket.send(response);
        delete result;
    }

    return 0;
}
