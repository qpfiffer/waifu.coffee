#include <msgpack.hpp>
#include <thread>
#include <zmq.hpp>
#include "waifu.h"
#include "utils.h"

using namespace kyotocabinet;
using namespace waifu;

int waifu::main_loop(int argc, char *argv[]) {
    scheduler mainScheduler;

    // Prepare our context and socket
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REP);
    zmq::socket_t scheduler_socket(context, ZMQ_PUSH);

    // Setup the main python 0mq interface
    if (argc < 2) {
        socket.bind(DEFAULT_URI);
    } else {
        socket.bind(argv[1]);
    }

    // Setup the main scheduler communication layer
    scheduler_socket.bind(SCHEDULER_URI);

    cout << "[-] Socket bound. Main thread init, yo." << endl;

    // Now that our publishing channel is setup, fire up the subscriber
    mainScheduler.spawn_thread();

    while (true) {
        // Wait for next request from client
        zmq::message_t request;
        socket.recv(&request);

        msgpack::object obj;
        utils::zmq_to_msgpack(&request, &obj);

        cout << "[-] Main loop received: " << obj << endl;

        // TODO: Figure out if the request was meant for the scheduler or not
        // Propogate request to scheduler
        scheduler_socket.send(request);

        map<string,bool> to_return = {{"success",true}};

        msgpack::sbuffer mresponse;
        msgpack::pack(&mresponse, to_return);

        // Convert to 0mq message
        zmq::message_t response(mresponse.size());
        memcpy((void *)response.data(), mresponse.data(), mresponse.size());
        socket.send(response);
        // TODO: Destroy message to free memory
    }

    return 0;
}
