#include <iostream>
#include <kcpolydb.h>
#include <msgpack.hpp>
#include <zmq.hpp>
#include "waifu.h"

using namespace kyotocabinet;
using namespace waifu;

vector<job> *waifuScheduler::get_jobs_from_db(PolyDB *db) {
    std::string value;
    if (!db->get(JOBS_QUEUE_NAME, &value)) {
        cerr << "get error: " << db->error().name() << endl;
        return NULL;
    }

    // Deserialize
    msgpack::unpacked msg;
    msgpack::unpack(&msg, value.data(), value.size());
    msgpack::object obj = msg.get();

    cout << "Current jobs: " << obj << endl;

    vector<job> *jobs = new vector<job>;
    obj.convert(jobs);

    return jobs;
}

bool waifuScheduler::new_query(job new_job) {
    std::cout << "Filepath: " << new_job["filepath"] << std::endl;
    PolyDB db;

    if (!db.open("jobs.kch", PolyDB::OWRITER | PolyDB::OCREATE)) {
        std::cerr << "open error: " << db.error().name() << std::endl;
        return false;
    }

    vector<job> *jobs = this->get_jobs_from_db(&db);
    if (jobs == NULL) {
        jobs = new vector<job>;
    }

    jobs->push_back(new_job);

    // Serialize
    msgpack::sbuffer new_jobs_list;
    msgpack::pack(new_jobs_list, *jobs);

    if (!db.set(JOBS_QUEUE_NAME, new_jobs_list.data())) {
        std::cerr << "set error: " << db.error().name() << endl;
    }

    db.close();
    delete jobs;
    return true;
}

msgpack::sbuffer *waifuScheduler::process_request(msgpack::unpacked *request) {
    // Setup our response object
    std::map<std::string, bool> response_list;
    response_list["success"] = false;

    // Convert the request into something useful
    msgpack::object obj = request->get();

    std::map<std::string, std::string> conv_request;
    obj.convert(&conv_request);

    if (conv_request["cmd"] == "query") {
        std::string filepath = conv_request["filepath"];
        // BE EXPLICIT. B.E. EXPLICIT!
        job new_job = {{"hash", ""}, {"filepath", filepath}};
        bool ret = this->new_query(new_job);
        response_list["success"] = ret;
    } else {
        response_list["success"] = false;
    }

    msgpack::sbuffer *to_return = new msgpack::sbuffer;
    msgpack::pack(to_return, response_list);

    return to_return;
}

int waifu::main_loop(int argc, char *argv[]) {
    char *URI = argv[1];

    waifuScheduler mainScheduler;

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
