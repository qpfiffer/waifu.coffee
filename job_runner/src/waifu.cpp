#include <msgpack.hpp>
#include <zmq.hpp>
#include "waifu.h"

using namespace kyotocabinet;
using namespace waifu;

int waifu::main_loop(int argc, char *argv[]) {
    char *URI = argv[1];

    scheduler mainScheduler;

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
        msgpack::sbuffer *result = mainScheduler.process_request(&unpacked_body);

        // Copy result data into response buffer
        zmq::message_t response(result->size());
        memcpy((void *)response.data(), result->data(), result->size());

        // Send it back
        socket.send(response);
        delete result;
    }

    return 0;
}
