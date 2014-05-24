#include <assert.h>
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
    zmq::context_t context(2);
    zmq::socket_t socket(context, ZMQ_REP);
    zmq::socket_t scheduler_socket(context, ZMQ_REQ);

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
        assert(socket.recv(&request) == true);

        //msgpack::object *obj = NULL;
        //obj = utils::zmq_to_msgpack(&request);

        //cout << "[-] Main loop received: " << *obj << endl;
        //cout << "[-] Main loop received (raw): " << (char *)request.data() << endl;
        //delete obj;

        // TODO: Figure out if the request was meant for the scheduler or not
        // Propogate request to scheduler
        scheduler_socket.send(request);
        zmq::message_t request_2;
        assert(scheduler_socket.recv(&request_2) == true);

        // Send the response from the scheduler back to python
        socket.send(request_2);
    }

    return 0;
}
